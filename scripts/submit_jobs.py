#EXAMPLE: python scripts/submit_jobs.py -m <macroname> -v <tagname> -n 10000000 -e 1000000

import os, subprocess, sys, time, random, optparse, socket, getpass
sys.path.append(os.path.abspath(os.path.curdir))

import BackgroundReader

from config import MacrosList, NEvts, MaxNJobs, TAG, GEANT4VERSION, GEANT4CONFIG, ROOTCONFIG, QUEUE, CODEDIR, OUTDIR, BSUBOUTDIR, TMPDIR, MATERIALS, BUILDDIR

#############################################################################################
## THE CODE BELOW SHOULD NOT BE TOUCHED. IF YOU FIND BUGS PLEASE EMAIL THE SIMULATION GROUP##
#############################################################################################


JobCounter=0

script_template = """
#!/bin/bash
#this is a very simple example of submission script
TAG=%(TAG)s
CODEDIR=%(CODEDIR)s
WORKDIR=%(WORKDIR)s
OUTDIR=%(OUTDIR)s
BUILDDIR=%(BUILDDIR)s
BSUBOUTDIR=%(BSUBOUTDIR)s
MACRONAME=%(MACRO)s

mkdir -p ${WORKDIR}
rm -rf ${WORKDIR}/*

alias cmake="/ua9/soft/cmake-3.14.4-install/bin/cmake"
###for ROOT
source /ua9/soft/root-v6-12-06-install/bin/thisroot.sh
# for pyROOT
export LD_LIBRARY_PATH=$PYTHONDIR/lib:$LD_LIBRARY_PATH
export PYTHONPATH=$ROOTSYS/lib:$PYTHONPATH
## for geant4
source /ua9/soft/geant4.10.05.p01-install/bin/geant4.sh 

## CADMesh
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/ua9/soft/CADMesh-install/lib

## need to cd in /ua9 to mount the disk...
cd /ua9/data
cd /ua9/user
## enter workdir
cd ${WORKDIR}
#/ua9/soft/cmake-3.14.4-install/bin/cmake -DGeant4_DIR=%(GEANT4CONFIG)slib64/Geant4-%(GEANT4VERSION)s -Dcadmesh_DIR=/ua9/soft/CADMesh-install/lib/cmake/cadmesh-1.1.0/ ${CODEDIR}
#make
#the macro is copied from the place where it has been modified for the test
rsync -ra ${BUILDDIR}CYGNO ./
rsync -ra ${OUTDIR}bsub_workdir/${TAG}/${MACRONAME}.mac ./
mkdir -p ${OUTDIR}${BSUBOUTDIR}${TAG}
mkdir -p ${OUTDIR}bsub_logs/${TAG}
#./CYGNO ${MACRONAME}.mac &> ${OUTDIR}bsub_logs/${TAG}/${MACRONAME}.log
./CYGNO ${MACRONAME}.mac 
mv ${WORKDIR}/${MACRONAME}.root ${OUTDIR}${BSUBOUTDIR}${TAG}/
cd ${OUTDIR}
rm -rf ${WORKDIR}/
"""
EOFmessage='The simulation took:' #This must be one of the last messages in output in the log file. It is used to check if the log file is complete or not

###############################################################################################################################
#THE REMAINING CODE SHOULD NOT BE TOUCHED
###############################################################################################################################
def parseInputArgs():
    parser = optparse.OptionParser(description='Submission script configuration.')
    parser.add_option('-c', '--codedir', default=None,
                      help='Folder where the simulation code is stored')
    parser.add_option('-o', '--outdir', default=None,
                      help='Folder where the jobs output will be saved')
    parser.add_option('-t', '--tmpdir', default=None,
                      help='Folder where the simulation will be compiled and executed')
    parser.add_option('-v', '--tag', default=None,
                      help='Identifier of the version of the simulation')
    parser.add_option('-f', '--file', default=None,
                      help='Specify the file containing the radioactive processes')
    parser.add_option('-g', '--geo', default=None,
                      help='Specify the geometry used')
    parser.add_option('-k', '--confine', default=None,
                      help='Define the physical volume where the source is confined')
    parser.add_option('-m', '--macro', default=None,
                      help='It is the configuration that has to be runned. When --file is specified then macro is used as template. It can be also a list of macros separated by a comma.')
    parser.add_option('-n', '--nevts', default=None,
                      help='Specify the number of events (not considered when --file is used). It can be also a list of events separated by a comma. The lenght must be the same of the list of macros')
    parser.add_option('-e', '--nevtsperjob', default=None,
                      help='Specify the maximum number of events per job')
    parser.add_option('-r', '--retry', default=False, action='store_true',
                      help='It looks in the output folder and watch for missing files and resubmit them.')
    parser.add_option('-d', '--debug', default=False, action='store_true',
                      help='Prints additional information messages. The submission of jobs via qsub is disabled but the structure will be created.')

    (options, args) = parser.parse_args()
    return options

def query_yes_no():
    yes = set(['yes','y'])
    no = set(['no','n'])

    choice = raw_input().lower()
    if choice in yes:
        return True
    elif choice in no:
        return False
    else:
        print 'Please respond with yes or no'
        query_yes_no()

def ModifyMacro(name=CODEDIR+'/macro/RadioactiveDecayTEMPLATE.mac', newname=CODEDIR+'/macro/RadioactiveDecayTEMPLATEOUT.mac', changes={}):
    #This is used to change to modify the macro 'name' and save it as 'newname' with the changes defined in the dictionary 'changes'
    new_file = open(newname,'w')
    old_file = open(name)
    for line in old_file:
      line.strip()
      elements=line.split()
      if len(elements)==0:
	continue
      #setting outname equal to macro name
      if line.split()[0]=='/CYGNO/outfile' and 'Outname' in changes.keys():
        line=line.replace(elements[-1],changes['Outname'])
      elif line.split()[0]=='/run/beamOn' and 'NEvs' in changes.keys():
        line=line.replace(elements[-1],changes['NEvs'])
      elif line.split()[0]=='/gps/ion' and 'A' in changes.keys() and 'Z' in changes.keys():
        if 'IonicCharge' in changes.keys() and 'ExcitationEnergy[keV]' in changes.keys():
            line='/gps/ion %s %s %s %s\n'%(changes['Z'],changes['A'],changes['IonicCharge'],changes['ExcitationEnergy[keV]'])
        else:
            line='/gps/ion %s %s\n'%(changes['Z'],changes['A'])
        ##Note the 1 option in replace. In the unlucky case that the placeholder for A and Z has the same value this replacement prevent that both elements are replaced with A A or Z Z
        #line=line.replace(elements[-2],changes['Z'],1)
        #line=line.replace(elements[-1],changes['A'],1)
      elif line.split()[0]=='/gps/pos/confine' and 'SourcePos' in changes.keys():
        line=line.replace(elements[-1],changes['SourcePos'])
      elif line.split()[0]=='/CYGNO/shield/thick0' and 'thick0' in changes.keys():
        line=line.replace(elements[-2],changes['thick0'])
      elif line.split()[0]=='/CYGNO/shield/thick1' and 'thick1' in changes.keys():
        line=line.replace(elements[-2],changes['thick1'])
      elif line.split()[0]=='/CYGNO/shield/thick2' and 'thick2' in changes.keys():
        line=line.replace(elements[-2],changes['thick2'])
      elif line.split()[0]=='/CYGNO/shield/thick3' and 'thick3' in changes.keys():
        line=line.replace(elements[-2],changes['thick3'])
      elif line.split()[0]=='/CYGNO/shield/mat0' and 'mat0' in changes.keys():
        line=line.replace(elements[-1],changes['mat0'])
      elif line.split()[0]=='/CYGNO/shield/mat1' and 'mat1' in changes.keys():
        line=line.replace(elements[-1],changes['mat1'])
      elif line.split()[0]=='/CYGNO/shield/mat2' and 'mat2' in changes.keys():
        line=line.replace(elements[-1],changes['mat2'])
      elif line.split()[0]=='/CYGNO/shield/mat3' and 'mat3' in changes.keys():
        line=line.replace(elements[-1],changes['mat3'])
      elif line.split()[0]=='/random/setSeeds':
        seed1=int(random.random()*1000000000)
        seed2=int(random.random()*1000000000)
        line=line.replace(elements[-2],str(seed1))
        line=line.replace(elements[-1],str(seed2))
      new_file.write(line)
    new_file.close()
    old_file.close()

def GetGeometry(NGeo='1'):
    #return a list containing the thickness and the material for each part of the shielding
    geos={
        '1':{'thick0':'1.'  ,'thick1':'1.'  ,'thick2':'50.'  ,'thick3':'5.'   ,'mat0':'Air'  ,'mat1':'Air'  ,'mat2':'Water'   ,'mat3':'Cu',}, # 50 cm water + 5 cm Cu
        #'2':{'thick0':'1.'  ,'thick1':'1.'  ,'thick2':'50.'  ,'thick3':'5.'   ,'mat0':'Air'  ,'mat1':'Air'  ,'mat2':'PE'      ,'mat3':'Cu',}, # 50 cm PE + 5 cm Cu
        #'3':{'thick0':'1.'  ,'thick1':'1.'  ,'thick2':'1.'   ,'thick3':'10.'  ,'mat0':'Air'  ,'mat1':'Air'  ,'mat2':'Air'     ,'mat3':'Cu',}, # 10 cm Cu
        #'4':{'thick0':'1.'  ,'thick1':'1.'  ,'thick2':'1.'   ,'thick3':'5.'   ,'mat0':'Air'  ,'mat1':'Air'  ,'mat2':'Air'     ,'mat3':'Cu',}, # 5 cm Cu
        #'5':{'thick0':'1.'  ,'thick1':'1.'  ,'thick2':'1.'   ,'thick3':'20.'  ,'mat0':'Air'  ,'mat1':'Air'  ,'mat2':'Air'     ,'mat3':'Pb',}, # 20 cm Pb
        #'6':{'thick0':'1.'  ,'thick1':'1.'  ,'thick2':'1.'   ,'thick3':'10.'  ,'mat0':'Air'  ,'mat1':'Air'  ,'mat2':'Air'     ,'mat3':'Pb',}, # 10 cm Pb
        #'7':{'thick0':'1.'  ,'thick1':'1.'  ,'thick2':'20.'  ,'thick3':'5.'   ,'mat0':'Air'  ,'mat1':'Air'  ,'mat2':'Pb'     ,'mat3':'Cu',}, # 20 cm Pb + 5 cm Cu
        '2':{'thick0':'1.'  ,'thick1':'1.'  ,'thick2':'50.'  ,'thick3':'0.5'   ,'mat0':'Air'  ,'mat1':'Air'  ,'mat2':'Water'      ,'mat3':'Cu',}, # 50 cm water + 5 mm Cu
        '3':{'thick0':'1.'  ,'thick1':'1.'  ,'thick2':'1.'   ,'thick3':'50.'  ,'mat0':'Air'  ,'mat1':'Air'  ,'mat2':'Air'     ,'mat3':'Water',}, # 50 cm water
    }
    if NGeo not in geos.keys():
        print 'The geometry specified ( %s ) is not defined'%(NGeo)
        sys.exit()
    return geos[NGeo]

def GetMaterial(filename):
    for m in MATERIALS:
        if filename.startswith(m):            
            return m, MATERIALS[m]
    print 'WARNING! The file name %s does not match any of the materials listed in MATERIALS. Looking for the keyword Activity ...'%(filename)
    if 'Activity' in filename:
        return filename[:filename.find('Activity')],''
    print 'ERROR! The material could not be extracted from the name of the file: %s \n Please add the material to the list or place it in the name of the file followed by the keyword Activity'%(filename)
    sys.exit()

def GetSourcePosition(geometry,g4mat):
    pos=[]
    if g4mat == '':            
        print 'ERROR! Cannot determine the position of the source because the material is not defined'
        sys.exit()
    for g in geometry:
        if 'mat' in g and geometry[g]==g4mat:
            pos.append('Shield'+g[3:])
    return pos

def SubmitJob(MACRO='RadioactiveDecayTEMPLATEOUT', TEMPLATE='RadioactiveDecayTEMPLATE', NEvents='10000', changes={}):
  #create a macro 'MACRO' which is a modified copy of 'TEMPLATE' with a total number of events 'NEvents' and other changes
  global JobCounter
  print '############################################################################################################################################################################################'
  print MACRO
  options = parseInputArgs()

  DEBUGMODE=False
  if options.debug:
    DEBUGMODE=True
  
  NParts=1#number of jobs used to process all the events
  if int(NEvents)/int(NEvtsPerJob)>0:
    NParts=int(NEvents)/int(NEvtsPerJob)
    if int(NEvents)%int(NEvtsPerJob)>0:
      NParts+=1
  if NParts>MaxNJobs-JobCounter:
      print 'Cannot submit more jobs because the max number of jobs would be reached'
      return
  print 'number of parts the job is divided: ', NParts
  for p in range(0,NParts):
    if NParts==1:
      NEvs=NEvents
      Part=''
    elif p==NParts-1 and int(NEvents)%int(NEvtsPerJob)>0:          
      NEvs=str(int(NEvents)%int(NEvtsPerJob))
      Part='_part'+str(p)
    else:
      NEvs=NEvtsPerJob
      Part='_part'+str(p)
    print 'Events for part %s are %s'%(Part, NEvs)

    if options.retry:
        #Checking if the output and log exists
        if os.path.isfile(OUTDIR+'bsub_logs/'+TAG+'/'+MACRO+Part+'.log') and os.path.isfile(OUTDIR+BSUBOUTDIR+TAG+'/'+MACRO+Part+'.root'):
            continue
        #Checking for outputs without log files
        if not os.path.isfile(OUTDIR+'bsub_logs/'+TAG+'/'+MACRO+Part+'.log') and os.path.isfile(OUTDIR+BSUBOUTDIR+TAG+'/'+MACRO+Part+'.root'):
            print 'Cannot find the log: %s' %(OUTDIR+'bsub_logs/'+TAG+'/'+MACRO+Part+'.log')
            print 'But the output file exists anyway, so nothing is done'
            continue
        #Checking for jobs without output file
        if os.path.isfile(OUTDIR+'bsub_logs/'+TAG+'/'+MACRO+Part+'.log') and not os.path.isfile(OUTDIR+BSUBOUTDIR+TAG+'/'+MACRO+Part+'.root'):
            log_file = open(OUTDIR+'bsub_logs/'+TAG+'/'+MACRO+Part+'.log')
            iscomplete=False
            for line in log_file:
                if line.startswith(EOFmessage):
                    iscomplete=True
                    break
            if iscomplete:
                print 'Log file %s is complete: waiting 20s to see if an output appears' %(OUTDIR+'bsub_logs/'+TAG+'/'+MACRO+Part+'.log') 
                wait=0
                while not os.path.isfile(OUTDIR+BSUBOUTDIR+TAG+'/'+MACRO+Part+'.root') and wait<20:
                    time.sleep(1)
                    wait+=1
                if os.path.isfile(OUTDIR+BSUBOUTDIR+TAG+'/'+MACRO+Part+'.root'):
                    print 'Output has been generated and the job will not be resubmitted'
                    continue
                else:
                    print 'Resubmitting the job because no output has been found'
            else:
                #print 'The log file %s seems incomplete. The jobs is probably still running and will not be resubmitted.' %(OUTDIR+'bsub_logs/'+TAG+'/'+MACRO+Part+'.log')
                #continue
                print 'Resubmitting the job because no output has been found'

    changes['Outname']=MACRO+Part
    changes['NEvs']=NEvs
    #print changes
    ModifyMacro(CODEDIR+'/macro/'+TEMPLATE+'.mac', OUTDIR+'/bsub_workdir/'+TAG+'/'+MACRO+Part+'.mac', changes)

    kw = {}
    #WORKDIR has to be an unique name per job
    WORKDIR=TMPDIR+TAG+'/'+MACRO+Part #this is where the simulation should be compiled
    kw['WORKDIR']=WORKDIR
    kw['TAG']=TAG
    kw['CODEDIR']=CODEDIR
    kw['OUTDIR']=OUTDIR
    kw['BSUBOUTDIR']=BSUBOUTDIR
    kw['MACRO']=MACRO+Part

    kw['GEANT4VERSION']=GEANT4VERSION
    kw['GEANT4CONFIG']=GEANT4CONFIG
    kw['ROOTCONFIG']=ROOTCONFIG
    kw['BUILDDIR']=BUILDDIR

    script_filename = OUTDIR+'bsub_workdir/'+TAG+'/'+MACRO+Part+'.sh'
    log_filename = OUTDIR+'/bsub_logs/'+TAG+'/'+MACRO+Part+'.log'
    hostname = socket.gethostname()
    open(script_filename, 'w').write(script_template % kw)
    subprocess.call(['chmod', '+x',script_filename])

    JobCounter+=1


    #whitelist = '-m "farm-wn-10"' # farm-wn-11 farm-wn-12 farm-wn-13 farm-wn-14 farm-wn-15 farm-wn-16 farm-wn-17"'
    #blacklist = '"-R select[hname!=farm-wn-01 && hname!=farm-wn-02 && hname!=farm-wn-03 && hname!=farm-wn-04 && hname!=farm-wn-05 && hname!=farm-wn-06 && hname!=farm-wn-07 hname!=farm-wn-08 && hname!=farm-wn-09]"'
    #blacklist = '-R "select[hostname!=farm-wn-06]"'
    #blacklist =''
    subprocess.call(['bsub', '-q', QUEUE, '-o', log_filename, script_filename])
        #until there is no multithreading there is no reason to require 8 cores
        #subprocess.call(['qsub', '-q', 'gs', '-l', 'ncpus=8', '-d', TMPDIR, script_filename])
        #subprocess.call(['qsub', '-q', 'gs', '-l', 'ncpus=1', '-e', 'localhost:'+OUTDIR+'bsub_logs/'+TAG+'/'+MACRO+Part+'.err', '-o', 'localhost:'+OUTDIR+'bsub_logs/'+TAG+'/'+MACRO+Part+'.out', '-d', TMPDIR, script_filename]) #It is used to write in files any prompt output produced by the bash script but it doesn't work at the moment

def main():
    #initializing random seed with the time
    random.seed()
    print "starting the job submission with bsub..."   
 
    #to modify the global variables
    global MacrosList
    global NEvts
    global NEvtsPerJob
    global TAG
    global CODEDIR
    global OUTDIR
    global BSUBOUTDIR
    global TMPDIR
    #reading options
    options = parseInputArgs()
    if options.codedir:
        CODEDIR=options.codedir
    if CODEDIR.endswith('/'):
        CODEDIR=CODEDIR[:-1]
    if options.outdir:
        OUTDIR=options.outdir
    if not OUTDIR.endswith('/'):
        OUTDIR=OUTDIR+'/'
    if not os.path.isdir(CODEDIR):
        print 'ERROR: Code directory does not exist or is not found'
        return
    if not os.path.isdir(OUTDIR):
        print 'ERROR: Output directory does not exist or is not found'
        return

    if options.tmpdir:
        TMPDIR=options.tmpdir
    if not TMPDIR.endswith('/'):
        TMPDIR=TMPDIR+'/'
    if options.tag:
        TAG=options.tag
    if options.nevtsperjob:
        NEvtsPerJob=options.nevtsperjob
    if options.macro:
        MacrosList=[]
        for m in options.macro.split(','):
            MacrosList.append(m.strip())
    if options.nevts:
        if options.file:
            print 'Argument --nevts will be ignored because the number of events is read from file'
        else:
            NEvts=[]
            for n in options.nevts.split(','):
                if not n.strip().isdigit():
                    print 'nevts is not a list of integers!!'
                    return
                NEvts.append(n.strip())
            if len(MacrosList)!=len(NEvts):
                print 'ERROR: size of macro list '+str(len(MacrosList))+' is different than size of nevents list '+str(len(NEvts))
                return

    #The full shielding geometry can be red from file
    geometry={}
    if options.geo:
        geometry=GetGeometry(options.geo)
    #Getting the source, the number of events, the activity from the file
    sources={}
    if options.file:
        sources=BackgroundReader.ReadFile(options.file, CODEDIR+'/backgrounds/')

    #find the g4 material from the name of the file
    if options.file:
        mat,g4mat=GetMaterial(options.file)

    pos=[]
    #In the full shielding geometry, given the material and the geometry.
    if options.geo and options.file:
        pos=GetSourcePosition(geometry,g4mat)
    #The volume where the process is originated in case the confine option is used
    elif options.confine:
        for p in options.confine.split(','):
            pos.append(p.strip())

    try:
      os.makedirs(OUTDIR+'bsub_workdir')#this is the folder where modified macros and the script will be placed
    except OSError:
      pass
    try:
      os.makedirs(OUTDIR+'bsub_workdir/'+TAG)
    except OSError:
      pass
    try:
      os.makedirs(OUTDIR+'bsub_logs')
    except OSError:
      pass

    #security checks
    if os.path.isdir(OUTDIR+'bsub_logs/'+TAG) and not options.retry:
        print 'You are about to submit jobs for a TAG that has already been used. You could overwrite previous results or you could conflict with running jobs. If you want to resubmit jobs failed or not queued yet, use the option --retry. Do you want to proceed with submitting all the jobs for this tag? [y/n]'
        if not query_yes_no():
            print 'Aborted'
            sys.exit()
    elif options.retry:
        print 'You are about to resubmit the failed jobs for this TAG. Please be sure there are no jobs with this TAG submitted but not running yet, otherwise they will be duplicated. Proceed? [y/n]'
        if not query_yes_no():
            print 'Aborted'
            sys.exit()

    try:
      os.makedirs(OUTDIR+'bsub_logs/'+TAG)
    except OSError:
      pass
    try:
      os.makedirs(TMPDIR)
    except OSError:
      pass

    hostname = socket.gethostname()

    print 'Tag name: ',TAG

    #mode 3 and 3.1: 1 activity file, 1 geometry and one template. This can work even without geometry (3.1). Soemtimes you have a geometry defined in your macro that you don't want to change, but still run the configuration from the file. And you also don't want to change the position of the source. What you want to change is just the isotope and the number of events per isotope generated.
    if options.geo and options.file and options.macro and len(MacrosList)==1:
        print 'Executing the simulation using the template: ',MacrosList[0]
        for p in pos:
            for i in range(0,len(sources['Name'])):
                MACRO='Geo'+options.geo+'_'+mat+'_'+p+'_'+sources['Name'][i]
                changes={}
                changes['A']=sources['A'][i]
                changes['Z']=sources['Z'][i]
                if 'IonicCharge' in sources.keys():
                    changes['IonicCharge']=sources['IonicCharge'][i]
                if 'ExcitationEnergy[keV]' in sources.keys():
                    changes['ExcitationEnergy[keV]']=sources['ExcitationEnergy[keV]'][i]
                changes['SourcePos']=p
                for g in geometry:
                    changes[g]=geometry[g]
                SubmitJob(MACRO, MacrosList[0], sources['NEvents'][i],changes)
    elif options.file and options.macro and len(MacrosList)==1:
        print 'Executing the simulation using the template: ',MacrosList[0]
        for i in range(0,len(sources['Name'])):
            MACRO=mat+'_'+sources['Name'][i]
            changes={}
            changes['A']=sources['A'][i]
            changes['Z']=sources['Z'][i]
            if 'IonicCharge' in sources.keys():
                changes['IonicCharge']=sources['IonicCharge'][i]
            if 'ExcitationEnergy[keV]' in sources.keys():
                changes['ExcitationEnergy[keV]']=sources['ExcitationEnergy[keV]'][i]
            SubmitJob(MACRO, MacrosList[0], sources['NEvents'][i],changes)                
    #elif options.confine:
    #NEED TO ADD THIS OPTION
    #mode 4: 1 geometry to be changed in the list of files

    elif options.geo:
        if len(MacrosList)!=len(NEvts):
            print 'ERROR: size of macro list '+str(len(MacrosList))+' is different than size of nevents list '+str(len(NEvts))
            return
        for i in range(0,len(MacrosList)):
            TEMPLATEMACRO = MacrosList[i]
            MACRO='Geo'+options.geo+'_'+MacrosList[i]
            #mode 1,2
            changes={}
            SubmitJob(MACRO, TEMPLATEMACRO, NEvts[i],changes)
    else:
        if len(MacrosList)!=len(NEvts):
            print 'ERROR: size of macro list '+str(len(MacrosList))+' is different than size of nevents list '+str(len(NEvts))
            return
        for i in range(0,len(MacrosList)):
            MACRO=MacrosList[i]
            changes={}
            SubmitJob(MACRO, MACRO, NEvts[i],changes)
    return

main() 
