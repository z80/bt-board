PATH=%PATH%;C:\Programs\CSR\BlueSuite-2.4
rem pscli query data.psr data.psq 
rem BlueFlashCmd HC05
rem pscli merge data.psr

rem Backup chip settings
rem pscli -trans "SPITRANS=USB SPIPORT=1" dump pskeys-backup.psr
rem Save firmware backup (only for chips with flash, backup will include PS keys):
blueflashcmd -trans "SPITRANS=USB SPIPORT=1" -dump firmware-backup

rem Flash firmware from files csr-fw-backup.xpv and csr-fw-backup.xdv:
blueflashcmd -trans "SPITRANS=USB SPIPORT=1" HC05


