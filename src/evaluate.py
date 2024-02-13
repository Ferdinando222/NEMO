import data
import soundfile
import torch 

def run_file_through_model(model, infile, outfile, samplerate=44100):
    """
    read the sent file from disk, pass it through the model
    and back out to the other file 
    """
    indata = data.load_wav_file(infile, want_samplerate=samplerate)
    outputs = model.forward(torch.tensor(indata.reshape(1,-1,1)))
    outputs = outputs.cpu().detach().numpy().reshape(-1,1)
    soundfile.write(outfile, outputs, samplerate)
    
