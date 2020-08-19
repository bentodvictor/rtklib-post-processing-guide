# -*- coding: utf-8 -*-
#----------------------------------------------------------------------#
############       Universidade Federal de Santa Maria      ############
############    Instituto Nacional de Pesquisas Espaciais   ############
############               Centro de Tecnlogias             ############
############             Engenharia de Computação           ############
############              Victor Dallagnol Bento            ############
#----------------------------------------------------------------------#
import os, sys, math


#...............................................................          VARIÁVEIS GLOBAIS
listRinex = []      # Lista para arquivos RINEX
countRinex = 0      # Contador para quantidade de arquivos RINEX
listBases = []      # Lista para armazernar dados das bases
#JDAY = "010"
#JDAY = "041"
#JDAY = "069"
#JDAY = "100"
#JDAY = "130"
#JDAY = "161"
#JDAY = "191"
#JDAY = "222"
#JDAY = "253"
#JDAY = "283"
#JDAY = "314"
JDAY = "344"



# Caminho para abrir arquivos RINEX
pathRinex = "/YOUR_PATH/SIMULATIONS/"+ JDAY + "/RINEX/"
dirsRinex = os.listdir(pathRinex)

NumberOfSamples = 12      # Numero de amostras (2h em 2h = 12 amostras no dia)

listPos = []      # Lista para arquivos .POS de cada pasta (x, y, z, nome)

save = 0    # Para salvar nome e hora somente uma vez na lista final

# Caminho para abrir arquivos POS de cada correção
pathPos1 = "/YOUR_PATH/SIMULATIONS/"+ JDAY + "/IGS/"
dirsPos1 = os.listdir(pathPos1)
pathPos2 = "/YOUR_PATH/SIMULATIONS/"+ JDAY + "/Klobuchar/"
dirsPos2 = os.listdir(pathPos2)
pathPos3 = "/YOUR_PATH/SIMULATIONS/"+ JDAY + "/SUPIM/"
dirsPos3 = os.listdir(pathPos3)
pathPos4 = "/YOUR_PATH/SIMULATIONS/"+ JDAY + "/DualFreq/"
dirsPos4 = os.listdir(pathPos4)
# listas para armazernar informações de cada posicionamento de cada estação de 2h-2h
listCorrections = []
#...............................................................



#...............................................................          FUNÇÃO PRINCIPAL
def main():
    global listRinex, countRinex, listBases, pathRinex, dirsRinex, listPos, pathPos1, pathPos2, pathPos3, pathPos4, pathPos5, dirsPos1, dirsPos2, dirsPos3, dirsPos4, dirsPos5, save    # Para utilizar e editar variáveis globais
    
    # Leitura de todos os arquivos do diretório PATHDIR e armazenamento em Lista
    for file in dirsRinex:
        listRinex.insert(countRinex, file)
        countRinex += 1

    # For para leitura de todas as distancias dos arquivos RINEX
    iRinex = 0
    while iRinex < countRinex:
        
        readECEF = pathRinex + listRinex[iRinex]    # Caminho para leitura do arquivo RINEX
        line_num = 0            # Leitura do arquivo RINEX e busca por linhas que contém XYZ
        f = open(readECEF)
        for line in f:
            if "APPROX POSITION XYZ" in line:
                xyzBase = line  # Armazena linha que contém XYZ na variavel xyzBase
                break
            else:
                line_num += 1
        
        # Separa string por espaço (" ") e grava em lista listXYZ
        listXYZ = []
        listXYZ = list(xyzBase.split(" "))    # Separa elementos por " " e transforma em lista
        
        auxBase = []
        for index in listXYZ:       # Laço para percorrer Lista:
            if index == '':         # Se valor do índice da lista for '' (vazio), lê o próximo
                pass
            else:
                auxBase.append(index)   # Se valor for diferente, adicionar valor (x, y ou z) a lista auxiliar auxBase
        
        # Remove parte da lista que não é X, Y e Z
        auxBase.remove("APPROX")
        auxBase.remove("POSITION")
        auxBase.remove("XYZ\n")
        
        auxBase.append(listRinex[iRinex])       # Adiciona nome da base ao ultimo índice da lista
        listBases.append(auxBase)               # Adiciona lista com dados da base (x, y, z e nome) para Lista de Bases (lista de listas)
        #print auxBase
        iRinex += 1

    listPos = readAmountPOS(pathPos1, dirsPos1)

    
    # Esquema para rodar todas as correções, entrando em todas os diretórios
    countCorrection = 0
    if countCorrection == 0:            # IGS
        findCoordenates(pathPos1)
        countCorrection += 1
        # Já salvou na lista final o nome de cada .POS e as horas das amostras
        save = 1    
    if countCorrection == 1:            # KLOBUCHAR
        findCoordenates(pathPos2)
        countCorrection += 1
    if countCorrection == 2:            # SUPIM
        findCoordenates(pathPos3)
        countCorrection += 1
    if countCorrection == 3:            # DUAL FREQUENCY
        findCoordenates(pathPos4)
        countCorrection += 1
    
    print "   Estação " + "       Hora  " + "\t   GIM" + "\t\tKLOBUCHAR" + "\t   SUPIM-DAVS" + "\t    DUAL FREQUENCY"
    for idx in listCorrections:
        if idx[5] > 5.0 or idx[5] == -1.0:
            del(idx[5])
            del(idx[4])
            del(idx[3])
            del(idx[2])
            del(idx[1])
            del(idx[0])

    cdf = 0
    tdf = 0
    for idx in listCorrections:
        if idx != []:
            print idx
            if idx[2] != -1.0:
                tdf += idx[2]
                cdf += 1 

    #print tdf/cdf
    #del(listCorrections[0])

#...............................................................



#...............................................................          LISTAGEM DOS ARQUIVOS .POS
def readAmountPOS(pathPos_, dirsPos_):
    countPos = 0      # Contador para quantidade de arquivos .POS de cada pasta
    listPos_ = []
    
    # Leitura de todos os arquivos do diretório PATHDIR e armazenamento em Lista
    for file in dirsPos_:
        listPos_.insert(countPos, file)
        countPos += 1
    return listPos_
#...............................................................



#...............................................................          LEITURA DOS ARQUIVOS .POS
def findCoordenates(pathPos):
    global listCorrections, save
    
    counterPos = 0      # Contador para arquivos .POS
    eptStr = "0.0000  0.0000  0.0000  0.0000"
    
    # fazer for para ler cada .pos da determinada correção  
    while counterPos < countRinex:
        # Auxiliares
        saveLine = []       # Lista para salvar as linhas específicas das horas analisadas (de hora em hora, ou 2h em 2h, por ex)
        aux = []
        listAux = []
        readPOS = pathPos + listPos[counterPos]     # Caminho do arquivo .POS
    
        saveLine.insert(0, eptStr)
        saveLine.insert(1, eptStr)
        saveLine.insert(2, eptStr)
        saveLine.insert(3, eptStr)
        saveLine.insert(4, eptStr)
        saveLine.insert(5, eptStr)
        saveLine.insert(6, eptStr)
        saveLine.insert(7, eptStr)
        saveLine.insert(8, eptStr)
        saveLine.insert(9, eptStr)
        saveLine.insert(10, eptStr)
        saveLine.insert(11, eptStr)
        
        # for para leitura e armazenamento das linhas de 2h em 2h
        f = open(readPOS)
        for line in f:              # Tolerancia de 10 minutos entre as amostras, sempre salva a última sobreescrevendo as anteriores
            if " 00:0" in line:
                saveLine.pop(0)
                saveLine.insert(0, line)
        
            if " 02:0" in line:
                saveLine.pop(1)
                saveLine.insert(1, line)
                    
            if " 04:0" in line:
                saveLine.pop(2)
                saveLine.insert(2, line)
                    
            if " 06:0" in line:
                saveLine.pop(3)
                saveLine.insert(3, line)
                    
            if " 08:0" in line:
                saveLine.pop(4)
                saveLine.insert(4, line)
                    
            if " 10:0" in line:
                saveLine.pop(5)
                saveLine.insert(5, line)
                    
            if " 12:0" in line:
                saveLine.pop(6)
                saveLine.insert(6, line)
                
            if " 14:0" in line:
                saveLine.pop(7)
                saveLine.insert(7, line)
                    
            if " 16:0" in line:
                saveLine.pop(8)
                saveLine.insert(8, line)
                    
            if " 18:0" in line:
                saveLine.pop(9)
                saveLine.insert(9, line)
                
            if " 20:0" in line:
                saveLine.pop(10)
                saveLine.insert(10, line)
                    
            if " 22:0" in line:
                saveLine.pop(11)
                saveLine.insert(11, line)
                    
        counterLine = 0     # Contador para cada linha específica
        while counterLine < NumberOfSamples:     # Como serão analisadas amostras de 2h em 2h teremos 12 amostras
            aux = [x.strip() for x in saveLine[counterLine].split("  ")]        # separa por espaços duplos ("  ") string e salva em lista aux
            for i_ in aux:
                if i_ == '':        # Elimina posições inválidas da lista
                    aux.remove('')

            aux[0] = listPos[counterPos]    # Sobreescreve primeiro elemento da lista (dia e hora no formato .POS - desncessários) pelo nome da estação e salva em listAux
            listAux.append(aux[0])
            if counterLine == 0:            # Salva hora na listAux e atualia índice padrão de escrita saveInList
                listAux.append("00:00")
                saveInList = (12*counterPos)
            if counterLine == 1:
                listAux.append("02:00")
                saveInList = (12*counterPos)+1
            if counterLine == 2:
                listAux.append("04:00")
                saveInList = (12*counterPos)+2
            if counterLine == 3:
                listAux.append("06:00")
                saveInList = (12*counterPos)+3
            if counterLine == 4:
                listAux.append("08:00")
                saveInList = (12*counterPos)+4
            if counterLine == 5:
                listAux.append("10:00")
                saveInList = (12*counterPos)+5
            if counterLine == 6:
                listAux.append("12:00")
                saveInList = (12*counterPos)+6
            if counterLine == 7:
                listAux.append("14:00")
                saveInList = (12*counterPos)+7
            if counterLine == 8:
                listAux.append("16:00")
                saveInList = (12*counterPos)+8
            if counterLine == 9:
                listAux.append("18:00")
                saveInList = (12*counterPos)+9
            if counterLine == 10:
                listAux.append("20:00")
                saveInList = (12*counterPos)+10
            if counterLine == 11:
                listAux.append("22:00")
                saveInList = (12*counterPos)+11
          
          # Calculo da distancia euclidiana para determinada hora
            for idx in listBases:
                if idx[3][:4] == listPos[counterPos][:4]:       # Buca estação referente ao arquivo .POS para pegar coordenadas e fazer o cálculo
                    if aux[1] == "0.0000" and aux[2] == "0.0000" and aux[3] == "0.0000":
                        d = -1.0
                        listAux.append(d)
                    else:
                        d_1 = (float(aux[1])-float(idx[0]))**2
                        d_2 = (float(aux[2])-float(idx[1]))**2
                        d_3 = (float(aux[3])-float(idx[2]))**2
                        d = math.sqrt(d_1 + d_2 + d_3)
                        listAux.append(d)                           # Salva na lista ListAux
                    if save == 1:   # Depois que efetua a primeira correção, os nomes das estações e as horas já estão salvos na lista final (listCorrections), basta apenas adicionar a ditância da nova correção á listCorrections.
                        listCorrections[saveInList].append(d)
                else:
                    pass
            if save == 0:       # Se ainda verificou nenhuma correção, salva nomes e horas das estações na lista final listCorrections
                listCorrections.append(listAux)
            # Limpa vetores e atualiza contadores
            aux = []
            listAux = []
            counterLine += 1
        
        counterPos += 1
#...............................................................



#...............................................................          CHAMADA DA FUNÇÃO PRINCIPAL
if __name__ == '__main__':
    main() # chamada da função main
#...............................................................
