import torch
import torch.nn as nn
import json
import miscfuncs


#CREATE A CLASS WITH DIFFERENT NETWORK (NOT COMBINING THEM)
class SimpleModel(nn.Module):
    def __init__(self, hidden_size=32, num_layers=1, model_type='LSTM'):
        super().__init__()
        self.model_type = model_type
        if self.model_type == 'LSTM':
            self.rnn = nn.LSTM(1, hidden_size, num_layers, batch_first=True)
        elif self.model_type == 'GRU':
            self.rnn = nn.GRU(1, hidden_size, num_layers, batch_first=True)
        elif self.model_type == 'CNN':
            self.cnn = nn.Conv1d(1, hidden_size, kernel_size=3, padding=1)
        else:
            raise ValueError("Invalid model type. Choose from 'LSTM', 'GRU', or 'CNN'")
        
        self.dense = nn.Linear(hidden_size, 1)
        self.drop_hidden = True

    def zero_on_next_forward(self):
        self.drop_hidden = True 
    
    def forward(self, x):
        if self.drop_hidden:
            batch_size = x.size(0)
            if self.model_type == 'LSTM' or self.model_type == 'GRU':
                h_shape = [self.rnn.num_layers, batch_size, self.rnn.hidden_size]
                hidden = torch.zeros(h_shape).to(x.device)
                if self.model_type == 'LSTM':
                    cell = torch.zeros(h_shape).to(x.device)
                    x, _ = self.rnn(x, (hidden, cell))
                else:
                    x, _ = self.rnn(x, hidden)
            elif self.model_type == 'CNN':
                x = x.permute(0, 2, 1)  # Change shape to [batch, channels, sequence]
                x = self.cnn(x)
                x = x.permute(0, 2, 1)  # Restore original shape
            self.drop_hidden = False
        else:
            if self.model_type == 'LSTM' or self.model_type == 'GRU':
                x, _ = self.rnn(x)
            elif self.model_type == 'CNN':
                x = x.permute(0, 2, 1)
                x = self.cnn(x)
                x = x.permute(0, 2, 1)
        x = self.dense(x)
        return x

    def save_model(self, file_name, direc=''):
        if direc:
            miscfuncs.dir_check(direc)
        
        model_info = {
            'model_data': {
                'model': self.model_type,
                'input_size': self.rnn.input_size if hasattr(self, 'rnn') else 1,
                'output_size': self.dense.out_features,
                'num_layers': self.rnn.num_layers if hasattr(self, 'rnn') else 1,
                'hidden_size': self.rnn.hidden_size if hasattr(self, 'rnn') else self.cnn.out_channels,
                'bias_fl': True
            }
        }
        
        model_state = self.state_dict()
        for key in model_state:
            model_state[key] = model_state[key].tolist()
        
        model_info['state_dict'] = model_state
        
        miscfuncs.json_save(model_info, file_name, direc)


#CLASS THAT COMBINE RNN E CNN
class CombinedModel(nn.Module):
    def __init__(self, lstm_hidden_size=32, cnn_hidden_size=32, lstm_num_layers=1):
        super().__init__()
        self.lstm = nn.LSTM(1, lstm_hidden_size, lstm_num_layers, batch_first=True)
        self.cnn = nn.Conv1d(1, cnn_hidden_size, kernel_size=3, padding=1)
        self.dense = nn.Linear(lstm_hidden_size + cnn_hidden_size, 1)
        self.drop_hidden = True

    def zero_on_next_forward(self):
        self.drop_hidden = True 
    
    def forward(self, x):
        if self.drop_hidden:
            batch_size = x.size(0)
            h_shape = [self.lstm.num_layers, batch_size, self.lstm.hidden_size]
            hidden = torch.zeros(h_shape).to(x.device)
            cell = torch.zeros(h_shape).to(x.device)
            lstm_out, _ = self.lstm(x, (hidden, cell))
            lstm_out = lstm_out[:, -1, :]  # Prendi solo l'output dell'ultimo passo temporale
            
            x_permuted = x.permute(0, 2, 1)  # Cambia forma in [batch, channels, sequence]
            cnn_out = self.cnn(x_permuted)
            cnn_out = cnn_out.permute(0, 2, 1)  # Ripristina la forma originale
            
            self.drop_hidden = False
        
        concatenated_output = torch.cat((lstm_out, cnn_out), dim=1)
        x = self.dense(concatenated_output)
        return x
    
    def save_for_rtneural(self, outfile):
        class EncodeTensor(json.JSONEncoder):
            def default(self, obj):
                if isinstance(obj, torch.Tensor):
                    return obj.cpu().detach().numpy().tolist()
                return super().default(obj)

        with open(outfile, 'w') as json_file:
            json.dump(self.state_dict(), json_file, cls=EncodeTensor)

    def save_model(self, file_name, direc=''):
        if direc:
            miscfuncs.dir_check(direc)
        
        model_info = {
            'model_data': {
                'model': 'CombinedModel',
                'input_size': self.lstm.input_size,
                'output_size': self.dense.out_features,
                'num_layers':self.lstm.num_layers,
                'hidden_size':self.lstm.hidden_size,
                'bias_fl': True
            }
        }
        
        model_state = self.state_dict()
        for key in model_state:
            model_state[key] = model_state[key].tolist()
        
        model_info['state_dict'] = model_state
        
        miscfuncs.json_save(model_info, file_name, direc)
