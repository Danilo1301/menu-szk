Start-Process "powershell" -ArgumentList @(
    "-NoExit",
    "-Command",
    "Set-Location 'C:\PC\Projetos\GTA_Mobile\menu-szk\menu-szk-layout-tool'; npm run dev"
)