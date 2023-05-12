Import("env")
import shutil
import os.path

#rewritten by TG 9/16/22 to fix file not found errors
def noext2elf(source, target, env):

 sourcename = source[0].name     #source arg is a list, get 1st index (build output file).name
 firmware_name = env['PROGNAME']
 firmware_path = env.subst('$BUILD_DIR') + "/"
 #print(firmware_name)

# exit - if .elf file already exists
 if os.path.exists(firmware_path + firmware_name + ".elf"):
   print("noext2elf  .elf already exists - conversion skipped!")
   return
# exit - cannot rename .bin into .elf
 if ".bin" in sourcename:   
   print("noext2elf  .bin file passed in - conversion skipped!")
   return

# otherwise filename with NO extension may be an .elf file, let's rename it
 print("noext2elf  copying firmware to ELF...")
 #print(firmware_path)
 shutil.copyfile(firmware_path + firmware_name, firmware_path + firmware_name + ".elf")
 print("noext2elf  elf file created.")
 #env.Replace($PROGNAME="firmware_%s" % "elf" )

env.AddPostAction("buildprog", noext2elf)
env.AddPreAction("upload", noext2elf) # added to make .elf file before uploading when only running upload