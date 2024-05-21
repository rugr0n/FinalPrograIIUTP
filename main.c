#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PRODUCTS 4

// Definicion de la estructura de Producto
typedef struct {
  char code[4];
  char name[20];
  int price;
  int stock;
} Product;

// Inicializacion de la lista de productos
Product products[MAX_PRODUCTS] = {{"123", "Camisa", 1500, 10},
                                  {"456", "Pantalon", 2500, 13},
                                  {"789", "Medias", 1000, 18},
                                  {"234", "Chaqueta", 4200, 9}};

int efectivo = 20000; // Variable para el dinero en efectivo
int tarjeta = 10000;  // Variable para el dinero en tarjeta

// Funcion para limpiar la consola segun el sistema operativo
void clearConsole() {
#ifdef _WIN32
  system("cls"); // Windows
#else
  system("clear"); // Unix-like linux
#endif
}

// Funcion para esperar hasta que el usuario presione Enter
void waitEnter() {
  printf("\n-> Presione Enter para volver al menu principal: ");
  while (getchar() != '\n')
    ; // Esperar a que el usuario presione Enter
  clearConsole();
}

// Funcion para limpiar el buffer de entrada
void clearInputBuffer() {
  int c;
  while ((c = getchar()) != '\n' && c != EOF)
    ;
}

// Funcion para verificar las credenciales del usuario
bool login(const char *username, const char *password) {
  FILE *file = fopen("users.txt", "r");
  if (file == NULL) {
    perror("Error al abrir el archivo de usuarios");
    return false;
  }

  char fileUsername[20];
  char filePassword[20];
  bool loggedIn = false;

  while (fscanf(file, "%s %s", fileUsername, filePassword) == 2) {
    if (strcmp(username, fileUsername) == 0 &&
        strcmp(password, filePassword) == 0) {
      loggedIn = true;
      break;
    }
  }

  fclose(file);

  return loggedIn;
}

// Funcion para escribir una transaccion en un archivo
void writeTransaction(const char *filename, const char *transactionInfo) {
  FILE *file = fopen(filename, "a");
  if (file == NULL) {
    perror("Error al abrir el archivo de transacciones");
    return;
  }

  fprintf(file, "%s\n", transactionInfo);
  fclose(file);
}

// Funcion para actualizar el stock de un producto
bool updateStock(const char *code, int units, bool isSale) {
  for (int i = 0; i < MAX_PRODUCTS; i++) {
    if (strcmp(products[i].code, code) == 0) {
      if (isSale) {
        if (products[i].stock >= units) {
          products[i].stock -= units;
          return true; // Stock actualizado exitosamente
        } else {
          printf("No hay suficientes unidades disponibles.\n");
          return false; // Stock insuficiente
        }
      } else {
        products[i].stock += units;
        return true; // Stock actualizado exitosamente
      }
    }
  }
  printf("Codigo de producto no encontrado.\n");
  return false; // Producto no encontrado
}

// Funcion para mostrar la lista de productos
void displayProductList() {
  printf("\n---- Lista de productos ----\n");
  for (int i = 0; i < MAX_PRODUCTS; i++) {
    printf(" Cod Producto: %s - %s - Precio x Und: $%d - Unidades: %d\n",
           products[i].code, products[i].name, products[i].price,
           products[i].stock);
  }
}

// Funcion para vender un producto
void sellProduct() {
  printf("\n---- Vender producto ----\n");
  displayProductList();

  char code[4];
  int units;
  int paymentMethod;

  printf("\n-> Ingrese codigo de producto (3 caracteres): ");
  scanf("%3s", code);
  clearInputBuffer(); // Limpiar cualquier entrada adicional en el buffer

  printf("-> Unidades a vender: ");
  scanf("%d", &units);

  printf("-> Medio de pago (1: efectivo, 2: tarjeta): ");
  scanf("%d", &paymentMethod);

  if (paymentMethod != 1 && paymentMethod != 2) {
    printf("Opcion de medio de pago invalida.\n");
    return;
  }

  // Encontrar el producto por su codigo para obtener su precio
  int productPrice = -1;
  for (int i = 0; i < MAX_PRODUCTS; i++) {
    if (strcmp(products[i].code, code) == 0) {
      productPrice = products[i].price;
      break;
    }
  }

  if (productPrice == -1) {
    printf("Codigo de producto no encontrado.\n");
    return;
  }

  // Validar y procesar la venta
  if (updateStock(code, units, true)) {
    int totalPrice = units * productPrice;

    printf("\nVenta realizada:\n");
    printf("Codigo Producto: %s\n", code);
    printf("Unidades Vendidas: %d\n", units);
    printf("Total a Pagar: $%d\n", totalPrice);

    // Actualizar efectivo o tarjeta segun el medio de pago
    if (paymentMethod == 1) {
      efectivo += totalPrice;
    } else {
      tarjeta += totalPrice;
    }

    // Registrar la venta en el archivo de transacciones
    char transactionInfo[100];
    snprintf(transactionInfo, sizeof(transactionInfo),
             "Venta - Codigo: %s, Unidades: %d, Precio Total: $%d", code, units,
             totalPrice);
    writeTransaction("ventas.txt", transactionInfo);
    writeTransaction("ventasdia.txt", transactionInfo);
  }

  printf("\n-> Desea realizar otra operacion? (s/n): ");
  char tryAgain;
  scanf(" %c", &tryAgain);
  clearInputBuffer(); // Limpiar el buffer de entrada
  if (tryAgain == 's' || tryAgain == 'S') {
    clearConsole();
    sellProduct(); // Vender otro producto si el usuario desea
  }
}

// Funcion para comprar un producto
void buyProduct() {
  printf("\n---- Comprar producto ----\n");
  displayProductList();

  char code[4];
  int units;
  int unitPrice;
  int paymentMethod;

  printf("\n-> Ingrese codigo de producto (3 caracteres): ");
  scanf("%3s", code);
  clearInputBuffer(); // Limpiar cualquier entrada adicional en el buffer

  printf("-> Unidades a comprar: ");
  scanf("%d", &units);

  // Encontrar el producto por su codigo para obtener su precio de venta
  int productPrice = -1;
  for (int i = 0; i < MAX_PRODUCTS; i++) {
    if (strcmp(products[i].code, code) == 0) {
      productPrice = products[i].price;
      break;
    }
  }

  if (productPrice == -1) {
    printf("Codigo de producto no encontrado.\n");
    return;
  }

  printf("-> Precio por unidad: ");
  scanf("%d", &unitPrice);

  // Verificar que el precio de compra no sea mayor al precio de venta
  if (unitPrice > productPrice) {
    printf("El precio de compra no puede ser mayor al precio de venta.\n");
    return;
  }

  printf("-> Medio de pago (1: efectivo, 2: tarjeta): ");
  scanf("%d", &paymentMethod);

  if (paymentMethod != 1 && paymentMethod != 2) {
    printf("Opcion de medio de pago invalida.\n");
    return;
  }

  int totalPrice = units * unitPrice;

  // Validar si la compra puede realizarse
  if (totalPrice > (paymentMethod == 1 ? efectivo : tarjeta)) {
    printf("No hay suficiente dinero disponible en el medio de pago "
           "seleccionado.\n");
    return;
  }

  // Procesar la compra y actualizar el stock
  if (updateStock(code, units, false)) {
    printf("\nCompra realizada:\n");
    printf("Codigo Producto: %s\n", code);
    printf("Unidades Compradas: %d\n", units);
    printf("Total Pagado: $%d\n", totalPrice);

    // Actualizar efectivo o tarjeta segun el medio de pago
    if (paymentMethod == 1) {
      efectivo -= totalPrice;
    } else {
      tarjeta -= totalPrice;
    }

    // Registrar la compra en el archivo de transacciones
    char transactionInfo[100];
    snprintf(transactionInfo, sizeof(transactionInfo),
             "Compra - Codigo: %s, Unidades: %d, Precio Total: $%d", code,
             units, totalPrice);
    writeTransaction("compras.txt", transactionInfo);
  }

  printf("\n-> Desea realizar otra operacion? (s/n): ");
  char tryAgain;
  scanf(" %c", &tryAgain);
  clearInputBuffer(); // Limpiar el buffer de entrada
  if (tryAgain == 's' || tryAgain == 'S') {
    clearConsole();
    buyProduct(); // Comprar otro producto si el usuario desea
  }
}

// Funcion para mostrar las ventas del dia
void showDailySales() {
  printf("\n---- Ventas del Dia ----\n");

  FILE *file = fopen("ventasdia.txt", "r");
  if (file == NULL) {
    printf("No hay ventas registradas hoy.\n");
    return;
  }

  char line[100];
  while (fgets(line, sizeof(line), file)) {
    printf("%s", line);
  }

  fclose(file);

  // Mostrar ventas totales en efectivo y tarjeta del dia
  printf("Total en Efectivo: $%d\n", efectivo);
  printf("Total en Tarjeta: $%d\n", tarjeta);
}

// Funcion principal
int main() {
  char username[20];
  char password[20];
  bool loggedIn = false;

  do {
    printf("****************************\n");
    printf("* LOGIN ALMACEN LAS MECHAS *\n");
    printf("****************************\n");

    printf("-> Digite usuario: ");
    scanf("%19s", username);

    printf("-> Digite contrasena: ");
    scanf("%19s", password);

    loggedIn = login(username, password);
    clearConsole();

    if (!loggedIn) {
      printf("\n* Error: Usuario o contrasena incorrectos.\n");
      printf("\n-> Intentar de nuevo? (s/n): ");
      char tryAgain;
      scanf(" %c", &tryAgain);
      clearConsole();
      if (tryAgain != 's' && tryAgain != 'S') {
        printf("Saliendo del programa...\n");
        return 0;
      }
    }
  } while (!loggedIn);

  int choice;
  do {
    clearConsole();

    printf("**********************\n");
    printf("* ALMACEN LAS MECHAS *\n");
    printf("**********************\n");
    printf("\n-- Menu de opciones --\n");
    printf("1. Listar Productos\n");
    printf("2. Vender Producto\n");
    printf("3. Comprar Producto\n");
    printf("4. Ventas del Dia\n");
    printf("5. Salir\n");
    printf("\n-> Seleccione opcion: ");
    scanf("%d", &choice);
    clearInputBuffer(); // Limpiar el buffer de entrada despues de leer la
                        // opcion

    clearConsole();

    switch (choice) {
    case 1:
      displayProductList();
      waitEnter();
      break;
    case 2:
      sellProduct();
      break;
    case 3:
      buyProduct();
      break;
    case 4:
      showDailySales();
      waitEnter();
      break;
    case 5:
      printf("Saliendo del programa...\n");
      return 0;
    default:
      printf("* Error: La opcion no existe.\n");
    }

  } while (true);

  return 0;
}
