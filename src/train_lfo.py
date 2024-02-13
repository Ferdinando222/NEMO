#%%
import torch
import json

import logging
import os
import pathlib
from typing import Dict, List, Optional
import  miscfuncs

import torch as tr
import torch.nn as nn
from torch import Tensor

from mod_extraction.model_lfo import LSTMEffectModel

logging.basicConfig()
log = logging.getLogger(__name__)
log.setLevel(level=os.environ.get("LOGLEVEL", "INFO"))


def save_model(effect_model, file_name, direc=''):
    if direc:
        miscfuncs.dir_check(direc)
    
    # Ottieni le informazioni sul modello
    model_info = {
        'model_data': {
            'model': 'SimpleRNN',
            'input_size': effect_model.model.lstm.input_size,
            'output_size': effect_model.model.fc.out_features,
            'unit_type': effect_model.model._get_name(),
            'num_layers': effect_model.model.lstm.num_layers,
            'hidden_size': effect_model.model.lstm.hidden_size,
            'bias_fl': effect_model.model.fc.bias.tolist()  # Converti il bias in una lista
        }
    }
    
    # Ottieni lo stato del modello e convertilo in liste
    model_state = effect_model.state_dict()
    for key in model_state:
        model_state[key] = model_state[key].tolist()
    
    # Aggiungi lo stato del modello al dizionario delle informazioni sul modello
    model_info['state_dict'] = model_state
    
    # Salva le informazioni del modello in un file JSON
    miscfuncs.json_save(model_info, file_name, direc)

class EffectModel(nn.Module):
    def __init__(self, weights_path: Optional[str] = None, n_hidden: int = 20, sr: float = 44100) -> None:
        super().__init__()
        self.sr = sr
        self.model = LSTMEffectModel(in_ch=1, out_ch=1, n_hidden=n_hidden, latent_dim=1)
        if weights_path:
            assert os.path.isfile(weights_path)
            log.info(f"Loading effect model weights: {weights_path}")
            self.model.load_state_dict(tr.load(weights_path))
        self.prev_phase = tr.tensor(0.0)

    def make_argument(self, n_samples: int, freq: float, phase: float) -> Tensor:
        argument = tr.cumsum(2 * tr.pi * tr.full((n_samples,), freq) / self.sr, dim=0) + phase
        return argument

    def forward(self,
                x: Tensor,
                lfo_rate: Tensor,
                lfo_depth: Tensor,
                lfo_stereo_phase_offset: Tensor) -> Tensor:
        arg_l = self.make_argument(n_samples=x.size(-1), freq=lfo_rate.item(), phase=self.prev_phase.item())
        next_phase = arg_l[-1] % (2 * tr.pi)
        self.prev_phase = next_phase
        arg_r = arg_l + lfo_stereo_phase_offset.item()
        arg = tr.stack([arg_l, arg_r], dim=0)
        lfo = (tr.cos(arg) + 1.0) / 2.0
        lfo *= lfo_depth
        lfo = lfo.unsqueeze(1)
        x = self.model(x, lfo)
        return x


# Carica il modello dai checkpoint
checkpoint = torch.load('../models/lstm_20__lfo_2dcnn_io_sa_25_25_no_ch_ln__egfx_ph__epoch_0_step_73123.ckpt',map_location=torch.device('cpu'))
state = checkpoint['state_dict']
for key in list(state.keys()):
    new_key = key.replace("effect_model.", "model.")
    state[new_key] = state.pop(key)
# Crea un'istanza del tuo modello
filtered_state = {k: v for k, v in state.items() if k.startswith('model.')}
effect_model = EffectModel()

# Carica i pesi e i parametri del modello dai checkpoint
effect_model.load_state_dict(filtered_state)

# Salva i pesi e i parametri del modello in un file JSON
save_model(effect_model,'model_prova.json')

# %%
