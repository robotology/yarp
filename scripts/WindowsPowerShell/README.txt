** INSTRUCTIONS **
It is possible to exploit tab-expansion features on Windows using the Windows PowerShell (PS) instead of the usual command prompt to run Yarp executables.
To do so, you have to:
1. Open a Windows PowersShell terminal (you can usually find it in Start -> All Programs -> Accessories -> Windows PowerShell -> Windows PowerShell)
2. Run the following cmdlet:
  Set-ExecutionPolicy unrestricted
  ** DO THIS AT YOUR OWN RISK **
3. Copy the files profile.ps1 and YarpTabExpansion.ps1 inside a WindowsPowerShell folder in your user "Documents" directory, i.e., on Windows 7:
  C:\Users\<username>\Documents\WindowsPowerShell\

  Alternatively, you can create a junction or symbolic link to this folder in the Yarp repository:
  cmd /c mklink /J c:\Users\<username>\Documents\WindowsPowerShell\ <YARP_ROOT>\scripts\WindowsPowerShell

  (use mklink /D to create a symbolic link instead of a junction). This can be handy to have always up-to-date scripts, but it would make it more difficult to customize your PowerShell profile if you want to add other features at some point.
4. Close the PS terminal and open a new one: your profile, with Yarp tab-expansion features, is automatically loaded at startup. Enjoy! :)

** TIPS & TRICKS **
To manage multiple shells, you can download and install the console2 tool from SourceForge ( http://sourceforge.net/projects/console/ ), then configure it so that, in the settings/tabs configuration tab, the "shell" field points to %SystemRoot%\SysWOW64\WindowsPowerShell\v1.0\powershell.exe  (or wherever the Powershell executable is on your machine).

