[Unit]
Description=Check if camera is available and get all new files
Requires=multi-user.target

[Service]
Type=idle
WorkingDirectory=%PATH_TO_SCRIPT%
ExecStart=%PATH_TO_SCRIPT%/runSyncCamera.sh
Restart=always

[Install]
WantedBy=multi-user.target