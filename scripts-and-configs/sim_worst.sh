########################################################
############        HOW RUN THIS SCRIPT     ############
########################################################

# Copy SIM.SH file, RTKPOST configuration file and ANTENNA file in the same folder that RINEX files. 

    # Folder to save output Files 
  SimOut=$'WorstCase'
    # Simulation data
  SimData=$'344'
    # Files
  RINEX='brdc3440.18n'
  CLK='igr20311.clk'
  ORBIT='igr20311.sp3'
    # Contador
  COUNT=1

# List all elements of folder that contain the RINEX files.
cd /home/bent0vict0r/Downloads/SIMULATIONS/$SimData/RINEX
ls > /home/bent0vict0r/Downloads/SIMULATIONS/$SimData/RinexList_WORST.txt
cd /home/bent0vict0r/Downloads/SIMULATIONS/$SimData/

# Read line by line all the RINEX files.
while read LINHA; do
	# Read the Rinex Observations.
  RINEX_OBS=$(echo $LINHA | awk '{print $1}')
  echo $COUNT
  COUNT=$((COUNT+1))
  	
  	# Give a name to output file referent to RINEX file.
  OUT=${RINEX_OBS/.18o/.pos}
  #NAV=${RINEX_OBS/.18o/.18n}
  
  #echo "$RINEX_OBS"
  #echo "$OUT"
  
  rnx2rtkp -k /home/bent0vict0r/Downloads/SIMULATIONS/$SimData/rtkpost_worst.conf -o /home/bent0vict0r/Downloads/SIMULATIONS/$SimData/$SimOut/$OUT /home/bent0vict0r/Downloads/SIMULATIONS/$SimData/RINEX/$RINEX_OBS /home/bent0vict0r/Downloads/SIMULATIONS/$SimData/$RINEX /home/bent0vict0r/Downloads/SIMULATIONS/$SimData/$CLK /home/bent0vict0r/Downloads/SIMULATIONS/$SimData/$ORBIT
done < RinexList_WORST.txt

rm RinexList_WORST.txt

echo "DONE!"
