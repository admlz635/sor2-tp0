# Trabajo Practico Inicial de Sistemas Operativos y Redes II
## Device Drivers - Char Device



## Implementación

### Constants

```c
DEVICE_NAME=UNGS // Constante con el nombre del dispositivo
BUFFER_MAX_SIZE 1024 // Tamaño maximo del mensaje que se "guarda" en el kernel
CAESAR_SHIFT 4 // Cantidad de posiciones a desplazar con `caesar`
```

### Funciones
```c
int init_module(void); // Se llama al cargar el modulo
 
void cleanup_module(void); // Se llama al bajar el modulo

static void caesar_encrypt(char *message, int key); // Se utiliza para encriptar el mensaje que se pasa por referencia


static int device_open(struct inode *inode, struct file *file); // Se invoca cuando un proceso quiere abrir el archivo del dispositivo

static int device_release(struct inode *inode, struct file *file); // Se invoca al cerrar el archivo del dispositivo

static ssize_t device_read(struct file *file, char *buffer, size_t length, loff_t *offset); // Se invoca cuando un proceso quiere leer desde el archivo del dispositivo

static ssize_t device_write(struct file *file, const char *buffer, size_t length, loff_t *offset); // Se invoca cuando un proceso escribe en el dispositivo

```

# Como correrlo

*__si ya se habia cargado previamente...__*

```bash
sudo rmmod mimodulo

sudo rm -rf /dev/UNGS
```

--------------
## Compilación/Construcción

```bash
make clean & make;
```

### Cargando el modulo

```bash
sudo insmod mimodulo.ko;
```

### Crear el `char device` y actulizar los permisos del archivo


#### *Creando character device*

```bash
sudo mknod /dev/UNGS c ${Major_number} 0
#revisar el major asignado en el log del kernel
```

#### *Estableciendo permisos del archivo*

```bash
sudo chmod 666 /dev/UNGS
```
## Pruebas locales

*__\* se dejaron comentados en el codigo unos prints por si fuera necesario hacer un debug__*

### Para escribir
```bash
echo Hello world > /dev/UNGS
#salida esperada en el log del kernel
[11275.220259] UNGS : Data leida desde [user] Hello world
```

### Para leer
```bash
cat /dev/UNGS
#salida esperada en la terminal (considerando que el valor de ${CAESAR_SHIFT} es 4)
Lipps asvph
```
### Ver log de mensajes del kernel

```bash
dmesg -w
#salida esperada en el log del kernel
[   71.674483] UNGS : Driver registrado
[   71.674485] UNGS : Major asignado 236
```