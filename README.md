# Introduce

Just some functions recorded here used on Windows,  
because of some win32 api extremely not easy to use directly,  
and also the human being (me) don't want to write those code.  
So, I write this header, and I will be continued to updated this file.  

# Current Function

## SetWin32Clipboard

Add some texts to the Clipboard.  

## GetCurProcessTopLevelWindow

Get the TopLevel Window's HWND (handle) of current process.  

## SetWindowIcon

Set win32 window icon, appear in window title, and Task view (Alt + Tab).  

## OpenExplorer

Open file explorer on a given path.  

## AllocConsoleWindow

Alloc a console window with quick edit mode disable.  
If the quick edit mode is on, mouse click in the console window will block the application.

## reg_INT_signal & wait_INT_signal

Register the INTERRUPT signal.
And then wait for Ctrl+C,the interrupt signal.
