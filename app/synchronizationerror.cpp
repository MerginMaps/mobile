/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

 #define _WIN32_WINNT 0x0500 // Es necesaria esta definicion para esconder ventana de consola

 #include "synchronizationerror.h"
 
 #include <windows.h> // Libreria que contiene las funciones de Winapi
 #define bt1 101
 #define bt2 102
 
 /*  Declaracion del procedimiento de windows  */
 
 LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);
 
 
 
 /*  Declaramos una variable de tipo char para guardar el nombre de nuestra aplicacion  */
 
 TCHAR NombreClase[] = TEXT("Estilos");
 
 HWND ventana1;           /* Manejador de la ventana*/
 
 MSG mensajecomunica;     /* Mensajes internos que se envian a la aplicacion */
 
 WNDCLASSEX estilo1;      /* Nombre de la clase para los estilos de ventana */
 
 
 
 int WINAPI WinMain (HINSTANCE hThisInstance, HINSTANCE /*hPrevInstance*/, LPSTR /*lpszArgument*/,int nCmdShow)
 
 
 {
 
     /* Creamos la estructura de la ventana indicando varias caracteristicas */
 
     estilo1.hInstance = hThisInstance;
 
     estilo1.lpszClassName = NombreClase;
 
     estilo1.lpfnWndProc = WindowProcedure;
 
     estilo1.style = CS_DBLCLKS;
 
     estilo1.cbSize = sizeof (WNDCLASSEX);
 
     estilo1.hIcon = LoadIcon (NULL, IDI_QUESTION);
 
     estilo1.hIconSm = LoadIcon (NULL, IDI_INFORMATION);
 
     estilo1.hCursor = LoadCursor (NULL, IDC_ARROW);
 
     estilo1.lpszMenuName = NULL;   /* Sin Menu */
 
     estilo1.cbClsExtra = 0;
 
     estilo1.cbWndExtra = 0;
 
     estilo1.hbrBackground = (HBRUSH) COLOR_WINDOW; /* Color del fondo de ventana */
 
 
 
     /* Registramos la clase de la ventana */
 
     if (!RegisterClassEx (&estilo1))
 
         return 0;
 
 
 
     /* Ahora creamos la ventana a partir de la clase anterior */
 
     ventana1 = CreateWindowEx (
 
            0,
 
            NombreClase,         /* Nombre de la clase */
 
            TEXT("Sincronizar informacion"),       /* Titulo de la ventana */
 
            WS_OVERLAPPEDWINDOW|WS_BORDER, /* Ventana por defecto */
 
            400,       /* Posicion de la ventana en el eje X (de izquierda a derecha) */
 
            70,       /* Posicion de la ventana, eje Y (arriba abajo) */
 
            300,                 /* Ancho de la ventana */
 
            100,                 /* Alto de la ventana */
 
            HWND_DESKTOP,
 
            NULL,                /* Sin menu */
 
            hThisInstance,
 
            NULL
 
            );
 
 
 
     /* Hacemos que la ventana sea visible */
 
     ShowWindow (ventana1, nCmdShow);
 
     ShowWindow(GetConsoleWindow(), SW_HIDE ); // Funcion para esconder la ventana de consola
 
 
 
     /* Hacemos que la ventan se ejecute hasta que se obtenga resturn 0 */
 
     while (GetMessage (&mensajecomunica, NULL, 0, 0))
 
     {
 
         /* Traduce mensajes virtual-key */
 
         TranslateMessage(&mensajecomunica);
 
         /* Envia mensajes a WindowProcedure */
 
         DispatchMessage(&mensajecomunica);
 
     }
 
 
 
     return mensajecomunica.wParam;
 
 }
 
 
 HWND boton1; 
 HWND boton2; 
 HWND encabezado;
 
 
 LRESULT CALLBACK WindowProcedure (HWND ventana1, UINT mensajecomunica, WPARAM wParam, LPARAM lParam)
 
 {
 
     switch (mensajecomunica) /* Manejamos los mensajes */
 
     {
 
         case WM_CLOSE: /* Que hacer en caso de recibir el mensaje WM_CLOSE*/
 
         DestroyWindow(ventana1); /* Destruir la ventana */
 
              break;
 
         case WM_DESTROY: /*Que hacer en caso de recibir el mensaje WM_DESTROY*/
 
         PostQuitMessage(0); /*salir de la aplicacion*/
 
             break;
 
             case WM_CREATE:
             
             boton1 = CreateWindowEx(0, TEXT("button"), TEXT("Si"), WS_VISIBLE | WS_CHILD | 0, 30, 20, 40, 20, ventana1,(HMENU)bt1,0,0); /*crear un boton*/
             boton2 = CreateWindowEx(0, TEXT("button"), TEXT("No"), WS_VISIBLE | WS_CHILD | 0, 200, 20, 40, 20, ventana1,(HMENU)bt2,0,0); /*crear un boton*/
             encabezado = CreateWindowEx(0, TEXT("static"), TEXT("Desea sincronizar los archivos"),WS_VISIBLE|WS_CHILD|SS_CENTER,0,0,300,20,ventana1,0,0,0); /*crear un encabezado*/
 
             break;
 
             case WM_COMMAND:
 
         if (LOWORD(wParam) == bt1) 
         {
             MessageBox(ventana1, TEXT("Archivos sincronizados"), TEXT("Sincronizacion"), MB_OK); /*mensaje de confirmacion*/
 
             ynchronizationError::SynchronizationError() = default;
 
 SynchronizationError::ErrorType SynchronizationError::errorType( int errorCode, const QString &errorMessage )
 {
   if ( errorCode == 400 )
   {
     // 'Another process is running. Please try later.'
     if ( errorMessage.contains( QStringLiteral( "Another process" ) ) )
     {
       return ErrorType::AnotherProcessIsRunning;
     }
 
     // 'Version mismatch'
     if ( errorMessage.contains( QStringLiteral( "mismatch" ) ) )
     {
       return ErrorType::VersionMismatch;
     }
   }
   else if ( errorCode == 403 )
   {
     // Cannot sync project
     return ErrorType::NoPermissions;
   }
   else if ( errorCode == 404 )
   {
     // Project no longer exists / is on different server
     return ErrorType::ProjectNotFound;
   }
   else if ( errorCode >= 500 )
   {
     // Exceptions in server code or maintenance mode
     return ErrorType::ServerError;
   }
 
   return ErrorType::UnknownError;
 }
 
 bool SynchronizationError::isPermanent( ErrorType errorType )
 {
   switch ( errorType )
   {
     case AnotherProcessIsRunning:
       [[fallthrough]];
     case VersionMismatch:
       [[fallthrough]];
     case ServerError: return false;
 
     default: return true;
             PostQuitMessage(0); /* Salir de la aplicación */
         }
         else if (LOWORD(wParam) == bt2) 
             {
                 MessageBox(ventana1, TEXT("Sincronizacion cancelada"), TEXT("Sincronizacion"), MB_OK); /*mensaje de cancelacion*/
                 PostQuitMessage(0); /**/
             }
 
             else if (LOWORD(wParam) == IDOK) /* Si se cierra la ventana */
 
            {
                 PostQuitMessage(0); /**/
             }
             
            
             break;
 
         default:  /* Tratamiento por defecto para mensajes que no especificamos */
 
             return DefWindowProc (ventana1, mensajecomunica, wParam, lParam);
 
     }
 
 return 0;
 
 }
 