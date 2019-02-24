from fs import *
import serial
import switch_case

#Important docs below
#https://docs.pyfilesystem.org/en/latest/reference/base.html
#https://docs.pyfilesystem.org/en/latest/info.html#info

#Note: all of the FS calls are actually unnecessary as they do nothing
#self.lock ensures that concurrent access doesn't mess everything up

class embedFS(FS):
    def __init__(serInstance):
        FS.__init__()
        self.ser = serInstance
        
    def getinfo(self,path,namespaces=None):
        FS.getinfo(self,path,namespaces)
        namespaces["basic"] = True
        info = {}
        with self._lock:
            ser.reset_input_buffer()
            for key in namespaces:
                ser.write(bytes("getinfo:"+key+":"+path+"|", 'utf-8'))#always retrive         
                while True:#embed will output 2 in a row to finish up
                    d = ser.read_until(b"|")
                    if len(d) <= 1:
                        break
                    key_val = d[:-1].split(":")
                    info["basic"]=info["basic"][key_val[0]] = key_val[1]
        return Info(info)
            
            
    def listdir(self,path):
        """Returns a list of """
        FS.listdir(self, path):
        dirs = []
        errs = {"SUCCESS":_noERR,"FAIL_NOPATH":_raiseRNF,"FAIL_NOTDIR":_raiseDE}
        with self._lock:
            ser.reset_input_buffer()
            ser.write(bytes("listdir:"+path+"|",'utf-8'))
            d = ser.read_until(b"|")
            errs.get(d[:-1])()
            while True:
                d = ser.read_until(b"|")
                if len(d) <= 1:
                    break
                dirs.append(d[:-1])
        return dirs

            
    def makedir(self,path,permissions = None,recreate = False):
        FS.makedir(self,path,permissions = None,recreate = False)
        errs = {"SUCCESS":_noERR,"FAIL_NOPATH":_raiseRNF,"FAIL_DIREXISTS":_raiseDEX}
        with self._lock:
            ser.reset_input_buffer()
            ser.write(bytes("makedir:"+path+"|",'utf-8'))
        if !reacreate:
            errs.get(d[:-1])()
        return self.opendir(path)
    
    def openbin(self,path,mode="r",buffering=-1,**options):
        FS.openbin(self,path,mode="r",buffering=-1,**options)
        errs = {"SUCCESS":_noERR,"FAIL_NOPATH":_raiseRNF,"FAIL_NOTFILE":_raiseFE}
        with self._lock:
            ser.write(bytes("open:"+path+"|"),'utf-8')
            d = ser.read_until(b"|")
            errs.get(d[:-1])()
            d = ser.read_until(b"|")
        binary = d[:-1]
        return binary
    
    def remove(self, path):
        FS.remove(self,path)
        with self._lock:
            ser.reset_input_buffer() #flush the input buffer so nothing extra disrupts commands
            ser.write(bytes("remove:"+path+"|",'utf-8'))
            d = ser.read_until(b"|")
        errs = {"SUCCESS":_noERR,"FAIL_NOPATH":_raiseRNF}
        errs.get(d[:-1])()
            
    def removedir(self, path):#this will run even if the directory is not empty, so must be recursively cleared
        FS.remove(self,path)
        with self._lock:
            ser.reset_input_buffer() #flush the input buffer so nothing extra disrupts commands
            ser.write(bytes("removedir:"+path+"|",'utf-8'))
            d = ser.read_until(b"|")
        errs = {"SUCCESS":_noERR,"FAIL_NOPATH":_raiseRNF,"FAIL_ROOT":_raiseRRE,"FAIL_NOTDIR":_raiseDE}
        errs.get(d[:-1])()
            
    def setinfo(self, path, info):
        with self._lock:
            
    def readbytes(self):
    def writebytes(self):

    def _raiseRRE(path):
        raise errors.RemoveRootError(path)
    def _raiseRNF(path):
        raise errors.ResourceNotFound(path)
    def _raiseDE(path):
        raise errors.DirectoryExpected(path)
    def _raiseDNE(path):
        raise errors.DirectoryNotEmpty(path)
    def _raiseFE(path):
        raise errors.FileExpected(path)
    def _raiseDEX(path):
        raise errors.DirectoryExists(path)
    def _noErr(path):
        pass    
