#%%
## This script trains an LSTM according
## to the method described in 
## A. Wright, E.-P. Damskägg, and V. Välimäki, ‘Real-time black-box modelling with recurrent neural networks’, in 22nd international conference on digital audio effects (DAFx-19), 2019, pp. 1–8.
import data
import models
import loss 
import utils_train
import torch 
from torch.utils.data import DataLoader
import evaluate
import os 
import wandb

# Define sweep config
sweep_configuration = {
    "method": "bayes",
    "name": "STMAE Project",
    "metric": {"goal": "minimize", "name": "val_loss"},
    "parameters": {
        "batch_size": {"values": [16,32,64]},
        "lstm_hidden_size":{"values": [20,32,64,100]},
        "learning_rate": {"values":[1e-3,1e-4,1e-5]},
        "layer":{"values":[1,2,3]},
    },
}

# Initialize sweep by passing in config.
sweep_id = wandb.sweep(sweep=sweep_configuration, project=f"STMAE_Project_5min")


def train():
    with wandb.init():
        # used for the writing of example outputs
        run_name="muff"
        # dataset : need an input and output folder in this folder
        #audio_folder = "../../data/audio_audacity_dist"
        audio_folder = "../data"
        assert os.path.exists(audio_folder), "Audio folder  not found. Looked for " + audio_folder
        # used to render example output during training
        test_file = "../data/input/test/guitar.wav"
        assert os.path.exists(test_file), "Test file not found. Looked for " + test_file
        lstm_hidden_size = wandb.config.lstm_hidden_size
        learning_rate = wandb.config.learning_rate
        batch_size = wandb.config.batch_size
        layers = wandb.config.layer
        max_epochs = 300

        print("Loading dataset from folder ", audio_folder)

        dataset = data.generate_dataset(audio_folder + "/input/train/", audio_folder + "/output/train/", frag_len_seconds=0.5)

        print("Looking for GPU power")
        device = utils_train.get_device()

        print("Splitting dataset")
        train_ds, val_ds, test_ds = data.get_train_valid_test_datasets(dataset,device)

        print("Creating model")
        model = models.SimpleModel(hidden_size=lstm_hidden_size,num_layers=layers,model_type='GRU').to(device)

        print("Creating data loaders")
        train_dl = DataLoader(train_ds, batch_size=batch_size, shuffle=True, generator=torch.Generator(device=device))
        val_dl = DataLoader(val_ds, batch_size=batch_size, shuffle=True, generator=torch.Generator(device=device))
        test_dl = DataLoader(test_ds, batch_size=batch_size, shuffle=True, generator=torch.Generator(device=device))

        print("Creating optimiser")
        # https://github.com/Alec-Wright/Automated-GuitarAmpModelling/blob/main/dist_model_recnet.py
        optimiser = torch.optim.Adam(model.parameters(), lr=learning_rate, weight_decay=1e-4)
        scheduler = torch.optim.lr_scheduler.ReduceLROnPlateau(optimiser, 'min', factor=0.5, patience=5, verbose=True)
        print("Creating loss functions")
        # https://github.com/Alec-Wright/CoreAudioML/blob/bad9469f94a2fa63a50d70ff75f5eff2208ba03f/training.py
        loss_functions = loss.LossWrapper()
        # now the training loop

        print("About to train")
        lowest_val_loss = 0
        best_loss = False
        for epoch in range(max_epochs):
            ep_loss = utils_train.train_epoch_interval(model, train_dl, loss_functions, optimiser, device=device)
            
            #ep_loss = myk_train.train_epoch(model, train_dl, loss_functions, optimiser, device=device)
            val_loss = utils_train.compute_batch_loss(model, val_dl, loss_functions, device=device)

            
            wandb.log({
                        "epoch":epoch,
                        "loss":ep_loss,
                        "val_loss":val_loss
            })
            
            # check if we have beaten our best loss to date
            if lowest_val_loss == 0:# first run
                lowest_val_loss = val_loss
            elif val_loss < lowest_val_loss:# new record
                lowest_val_loss = val_loss
                best_loss = True
            else:# no improvement
                best_loss = False
            if best_loss:# save best model so far
                print("Record loss - saving at ", epoch)
            model.save_model("model.json")
            if epoch % 50 == 0:# save an example processed audio file
                evaluate.run_file_through_model(model, test_file, audio_folder + "/" + run_name + str(epoch)+".wav")
            print("epoch, train, val ", epoch, ep_loss, val_loss)
        

if __name__=="__main__":
    #TRAINING 
    wandb.agent(sweep_id=sweep_id, project=f"STMAE_Project_5min",function=train)

# %%
