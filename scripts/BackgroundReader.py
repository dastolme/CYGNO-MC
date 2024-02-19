#This script is used to load the files with the radioactivity in the background folder. It simply load the content of the radioactive files into a dictionary to be then used by other parts of the code
import os, subprocess, sys, time
sys.path.append(os.path.abspath(os.path.curdir))

def ReadFile(filename, filedir='/ua9/user/dimperig/CYGNO/CYGNO-MC/backgrounds'):
  d={}
  keys=[]

  if not os.path.isdir(filedir):
    print ('ERROR: Directory %s does not exists' %(filedir))
    return
  if not os.path.isfile(filedir+filename):
    print ('ERROR: File %s does not exists' %(filename))
>>>>>>> 5ee9128a95cc0ea4eed06cf55254e7ed4e4e1aee
    return


  ref_file = open(filedir+filename)  
  for line in ref_file:
    line.strip()
    if line.startswith('#'):
      continue
    elements=line.split(',')
    if len(keys) == 0:
      #use the first line to make the dictionary keys
      for i in range(0,len(elements)):
        keys.append(elements[i].strip())
        d[elements[i].strip()]=[]
    else:
      for i in range(0,len(elements)):
        d[keys[i]].append(elements[i].strip())

  ref_file.close()
  return d
