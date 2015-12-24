PATH=%PATH%;C:\Programs\CSR\BlueSuite-2.4
#pscli query data.psr data.psq 
#BlueFlashCmd HC05
#pscli merge data.psr

# Backup chip settings
# pscli -trans "SPITRANS=USB SPIPORT=1" dump pskeys-backup.psr
# Save firmware backup (only for chips with flash, backup will include PS keys):
blueflashcmd -trans "SPITRANS=USB SPIPORT=1" -dump firmware-backup

# Flash firmware from files csr-fw-backup.xpv and csr-fw-backup.xdv:
blueflashcmd -trans "SPITRANS=USB SPIPORT=1" HC05


