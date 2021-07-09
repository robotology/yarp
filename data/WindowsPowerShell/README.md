#### INSTRUCTIONS

It is possible to exploit tab-expansion features on Windows using the **Windows PowerShell (PS)** instead of the usual command prompt to run YARP executables.

To do so, you have to:

1. Open a Windows PowersShell terminal (you can usually find it in `Start->All Programs->Accessories->Windows PowerShell->Windows PowerShell`).

2. Run the following _cmdlet_: `Set-ExecutionPolicy unrestricted`.

3. **Do this at your own risk**: copy the files `profile.ps1` and `YarpTabExpansion.ps1` inside a WindowsPowerShell folder in your user _"Documents"_ directory, i.e. on Windows 7: ` C:\Users\<username>\Documents\WindowsPowerShell`. Alternatively, you can create a junction or symbolic link to this folder in the YARP repository: `cmd /c mklink /J C:\Users\<username>\Documents\WindowsPowerShell\<YARP_ROOT>\scripts\WindowsPowerShell` (`mklink /D` to create a symbolic link instead of a junction). This can be handy to have always up-to-date scripts, but it would make it more difficult to customize your PowerShell profile if you want to add other features at some point.

4. Close the PS terminal and open a new one: your profile, with YARP tab-expansion features, is automatically loaded at startup. Enjoy! :)
