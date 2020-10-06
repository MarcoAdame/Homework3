import os
import ctypes

#clean and rebuild shared objects

def startup():

    print("------------------------------------------\nBuilding C objects...")

    thispath = os.path.abspath(os.path.dirname(__file__))

    #rebuild C shared objects (everything should be in this same directory!!)
    try:
        os.remove(os.path.join(thispath, "hashtable.o"))
        os.remove(os.path.join(thispath, "hashtable.so"))
    except:
        pass
    #try-except

    os.system("make all")

    print("Listo!\n------------------------------------------")
#startup

def hashTableOperations():

    #easy wrapper for C functions
    def wrap_function(lib, funcname, restype, argtypes):
        func = lib.__getattr__(funcname)
        func.restype = restype
        func.argtypes = argtypes
        return func
    #wrap_function
    libc = ctypes.CDLL("./hashtable.so")
    #wrap C's listReader function and set up its arg/return types
    InitHash = wrap_function(libc, "inicioRandom",       None,          None)
    finHash = wrap_function(libc, "finish",       None,          None)
    printTable  = wrap_function(libc, "printHash",      None,          None)
    insertKeyValue = wrap_function(libc, "insertHash", ctypes.c_bool, [ctypes.POINTER(ctypes.c_char), ctypes.c_uint])
    deleteKeyValue = wrap_function(libc, "deleteHash", ctypes.c_bool, [ctypes.POINTER(ctypes.c_char)])
    getValue = wrap_function(libc, "readHash",   ctypes.c_uint, [ctypes.POINTER(ctypes.c_char)])

    print("\nBienvenido a una verdadera Tabla de Hash\n")

    print("Paso 1: Iniciarlizar la Hash Table\n")
    InitHash()

    print("Añadir el elemento: modelo: {0}, costo: {1} ".format("Xiaomi", 199))
    insertKeyValue(ctypes.create_string_buffer(b"Xiaomi"), 199)

    print("Añadir el elemento: modelo: {0}, costo: {1} ".format("Note Pro", 199))
    insertKeyValue(ctypes.create_string_buffer(b"Note Pro"), 199)

    print("Añadir el elemento: modelo: {0}, costo: {1} ".format("Redmi 8", 199))
    insertKeyValue(ctypes.create_string_buffer(b"Redmi 8"), 199)

    printTable()

    print("Borrar a key:value pair: {0}".format("Xiaomi"))
    if deleteKeyValue(ctypes.create_string_buffer(b"Xiaomi")) == True:
        print("Borrado Exitoso")
    else:
        print("Borrado Fallido") 

    print("Borrar a key:value pair: {0}".format("Samsung"))
    if deleteKeyValue(ctypes.create_string_buffer(b"Samsung")) == True:
        print("Borrado Exitoso")
    else:
        print("Borrado Fallido") 
    
    printTable()
    
    finHash()
#hashTableOperations

def main():
    startup()
    hashTableOperations()
    
#main

if __name__ == "__main__":
    main()
#if

#eof
