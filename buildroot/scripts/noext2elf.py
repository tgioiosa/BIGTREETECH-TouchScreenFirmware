Import("env")
import shutil

def noext2elf(source, target, env):
 print("copying firmware to ELF...")
 
 firmware_name = env['PROGNAME']
 #print("firmware_name: " + firmware_name)
 firmware_path = env.subst('$BUILD_DIR') + "/"
 #print("firmware_path: " + firmware_path)
 shutil.copyfile(firmware_path + firmware_name + ".bin", firmware_path + firmware_name + ".elf")
 print("bin file created.")
 #env.Replace($PROGNAME="firmware_%s" % "elf" )

env.AddPostAction("buildprog", noext2elf)
env.AddPreAction("upload", noext2elf) # added to make .elf file before uploading when only running upload