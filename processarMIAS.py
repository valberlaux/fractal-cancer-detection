'''
Created on 18 Nov 2015

@author: leo
'''
import os

dadosPath = 'Dados-MIAS-imagens'
imagensPath = 'mamografias'
imagensBinarizadasPath = 'mamografiasBinarizadas'

dadosMIASDic = {}

def extrairDados():
    with open(dadosPath, encoding='utf-8', newline='\n') as dadosMIAS:
        for dadosImagem in dadosMIAS:
            dadosImagem = dadosImagem.split()    
            if len(dadosImagem) == 7:    
                dadosMIASDic[dadosImagem[0] + ".pgm"] = (dadosImagem[3],dadosImagem[4],dadosImagem[5],dadosImagem[6])
                
                
def processarTodas(): 
    imagensFileNames = os.listdir(imagensPath)
    for imagemFileName in imagensFileNames:
        if dadosMIASDic.get(imagemFileName,None) is not None:
            
            print('./out %s %s %s %s %s' % (os.path.join(imagensPath,imagemFileName),os.path.join(imagensBinarizadasPath,imagemFileName),\
                                                dadosMIASDic[imagemFileName][1],dadosMIASDic[imagemFileName][2],dadosMIASDic[imagemFileName][3]))
            
            os.system('./out %s %s %s %s %s' % (os.path.join(imagensPath,imagemFileName),os.path.join(imagensBinarizadasPath,imagemFileName),\
                                                dadosMIASDic[imagemFileName][1],dadosMIASDic[imagemFileName][2],dadosMIASDic[imagemFileName][3]))


if __name__ == "__main__":
    extrairDados()  
    os.system('make')
    processarTodas()
    
    