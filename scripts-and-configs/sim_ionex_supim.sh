#----------------------------------------------------------------------#
############       Universidade Federal de Santa Maria      ############
############    Instituto Nacional de Pesquisas Espaciais   ############
############               Centro de Tecnlogias             ############
############             Engenharia de Computação           ############
############              Victor Dallagnol Bento            ############
#----------------------------------------------------------------------#

# O Script deve estar no mesmo diretório dos "Arquivos Fixos" 

# Diretório de saída
SimOut=$'SUPIM'

#-----  Campo para alterações dependendo do dia que será simulado
    # Diretório da Simulação Simulation data
    SimData=$'344'
    # Arquivos Fixos
    RINEX='brdc3440.18n'
    CLK='igr20311.clk'
    ORBIT='igr20311.sp3'
#-----------------------------------

# Contador para indicar número de arquivos simulados
COUNT=1

# Lista arquivos RINEX de Observação (RINEX OBS) de cada estação 
cd /home/bent0vict0r/Downloads/SIMULATIONS/$SimData/RINEX
ls > /home/bent0vict0r/Downloads/SIMULATIONS/$SimData/RinexList_SUPIM.txt
cd /home/bent0vict0r/Downloads/SIMULATIONS/$SimData/

# Leitura da lista contendo o nome dos RINEX OBS
while read LINHA; do
    RINEX_OBS=$(echo $LINHA | awk '{print $1}')
    echo $COUNT
    COUNT=$((COUNT+1))
      	
    # Atribui o nome do arquivo RINEX OBS para o arquivo de saída mudando apenas sua extensão
    OUT=${RINEX_OBS/.18o/.pos}
      
    # PÓS-PROCESSAMENTO
    rnx2rtkp -k /home/bent0vict0r/Downloads/SIMULATIONS/$SimData/rtkpost_ionex_supim.conf -o /home/bent0vict0r/Downloads/SIMULATIONS/$SimData/$SimOut/$OUT /home/bent0vict0r/Downloads/SIMULATIONS/$SimData/RINEX/$RINEX_OBS /home/bent0vict0r/Downloads/SIMULATIONS/$SimData/$RINEX /home/bent0vict0r/Downloads/SIMULATIONS/$SimData/$CLK /home/bent0vict0r/Downloads/SIMULATIONS/$SimData/$ORBIT
done < RinexList_SUPIM.txt


# Remoção da Lista criada anteriormente com o nome dos RINEX OBS
rm RinexList_SUPIM.txt

echo "DONE!"
