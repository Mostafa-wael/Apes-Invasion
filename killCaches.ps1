$currentfolder = Get-Location
Get-ChildItem -Path $currentfolder -File -Include CMakeCache.txt, Makefile -Recurse | Remove-Item -Force -Verbose
Get-ChildItem -Path $currentfolder -Directory -Filter CMakeFiles -Recurse | Remove-Item -Force -Recurse -Verbose 
