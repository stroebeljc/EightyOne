object P3Drive: TP3Drive
  Left = 84
  Top = 0
  BorderIcons = [biSystemMenu, biMinimize]
  BorderStyle = bsToolWindow
  Caption = 'Drives'
  ClientHeight = 402
  ClientWidth = 374
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  Scaled = False
  OnClose = FormClose
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object OK: TButton
    Left = 292
    Top = 370
    Width = 75
    Height = 25
    Anchors = [akRight, akBottom]
    Caption = 'OK'
    TabOrder = 3
    OnClick = OKClick
  end
  object FloppyDriveGroup: TGroupBox
    Left = 8
    Top = 8
    Width = 358
    Height = 73
    Caption = 'Floppy Drives'
    TabOrder = 0
    object DriveALabel: TLabel
      Left = 9
      Top = 20
      Width = 38
      Height = 13
      Alignment = taRightJustify
      Caption = 'Drive A:'
    end
    object DriveBLabel: TLabel
      Left = 9
      Top = 44
      Width = 38
      Height = 13
      Alignment = taRightJustify
      Caption = 'Drive B:'
    end
    object DriveAText: TEdit
      Left = 56
      Top = 16
      Width = 185
      Height = 21
      Enabled = False
      ParentShowHint = False
      ReadOnly = True
      ShowHint = True
      TabOrder = 0
      Text = '< Empty >'
    end
    object DriveBText: TEdit
      Left = 56
      Top = 40
      Width = 185
      Height = 21
      Enabled = False
      ReadOnly = True
      TabOrder = 4
      Text = '< Empty >'
    end
    object DriveAFSBtn: TButton
      Left = 244
      Top = 16
      Width = 25
      Height = 23
      Caption = '...'
      TabOrder = 1
      OnClick = DriveAFSBtnClick
    end
    object DriveBFSBtn: TButton
      Left = 244
      Top = 40
      Width = 25
      Height = 23
      Caption = '...'
      TabOrder = 5
      OnClick = DriveBFSBtnClick
    end
    object DriveAEjectBtn: TButton
      Left = 308
      Top = 16
      Width = 41
      Height = 23
      Caption = 'Eject'
      TabOrder = 3
      OnClick = DriveAEjectBtnClick
    end
    object DriveBEjectBtn: TButton
      Left = 308
      Top = 40
      Width = 41
      Height = 23
      Caption = 'Eject'
      TabOrder = 7
      OnClick = DriveBEjectBtnClick
    end
    object DriveANewBtn: TButton
      Left = 272
      Top = 16
      Width = 33
      Height = 23
      Caption = 'New'
      TabOrder = 2
      OnClick = DriveANewBtnClick
    end
    object DriveBNewBtn: TButton
      Left = 272
      Top = 40
      Width = 33
      Height = 23
      Caption = 'New'
      TabOrder = 6
      OnClick = DriveBNewBtnClick
    end
  end
  object HardDriveGroup: TGroupBox
    Left = 8
    Top = 88
    Width = 358
    Height = 233
    Caption = 'Hard Drives'
    TabOrder = 1
    object HD1Label: TLabel
      Left = 18
      Top = 116
      Width = 28
      Height = 13
      Alignment = taRightJustify
      Caption = 'HD 1:'
    end
    object HD1CHS: TLabel
      Left = 12
      Top = 176
      Width = 35
      Height = 13
      Caption = 'C/H/S:'
    end
    object HD0Label: TLabel
      Left = 18
      Top = 28
      Width = 28
      Height = 13
      Alignment = taRightJustify
      Caption = 'HD 0:'
    end
    object HD0CHS: TLabel
      Left = 12
      Top = 84
      Width = 35
      Height = 13
      Caption = 'C/H/S:'
    end
    object HD1Text: TEdit
      Left = 56
      Top = 140
      Width = 185
      Height = 21
      Enabled = False
      ReadOnly = True
      TabOrder = 13
      Text = '< Empty >'
    end
    object HD1FSBtn: TButton
      Left = 244
      Top = 138
      Width = 25
      Height = 23
      Caption = '...'
      TabOrder = 14
      OnClick = HD1FSBtnClick
    end
    object HD1EjectBtn: TButton
      Left = 308
      Top = 138
      Width = 41
      Height = 23
      Caption = 'Eject'
      TabOrder = 16
      OnClick = HD1EjectBtnClick
    end
    object RedetectDrives: TButton
      Left = 212
      Top = 200
      Width = 137
      Height = 23
      Caption = 'Redetect Physical Drives'
      TabOrder = 22
      OnClick = RedetectDrivesClick
    end
    object HD1List: TComboBox
      Left = 56
      Top = 112
      Width = 185
      Height = 21
      Style = csDropDownList
      ItemHeight = 13
      TabOrder = 11
      OnChange = HD1ListChange
    end
    object HD1C: TEdit
      Left = 56
      Top = 172
      Width = 41
      Height = 21
      Enabled = False
      TabOrder = 17
      Text = '0'
    end
    object HD1H: TEdit
      Left = 104
      Top = 172
      Width = 41
      Height = 21
      Enabled = False
      TabOrder = 18
      Text = '0'
    end
    object HD1HUD: TUpDown
      Left = 146
      Top = 172
      Width = 25
      Height = 21
      Min = 0
      Orientation = udHorizontal
      Position = 0
      TabOrder = 19
      Wrap = False
      OnClick = HD1HUDClick
    end
    object HD1S: TEdit
      Left = 176
      Top = 172
      Width = 41
      Height = 21
      Enabled = False
      TabOrder = 20
      Text = '0'
    end
    object HD1SUD: TUpDown
      Left = 218
      Top = 172
      Width = 25
      Height = 21
      Min = 0
      Orientation = udHorizontal
      Position = 0
      TabOrder = 21
      Wrap = False
      OnClick = HD1HUDClick
    end
    object HD0List: TComboBox
      Left = 56
      Top = 24
      Width = 185
      Height = 21
      Style = csDropDownList
      ItemHeight = 13
      TabOrder = 0
      OnChange = HD0ListChange
    end
    object HD0Text: TEdit
      Left = 56
      Top = 52
      Width = 185
      Height = 21
      Enabled = False
      ParentShowHint = False
      ReadOnly = True
      ShowHint = True
      TabOrder = 2
      Text = '< Empty >'
    end
    object HD0FSBtn: TButton
      Left = 244
      Top = 50
      Width = 25
      Height = 23
      Caption = '...'
      TabOrder = 3
      OnClick = HD0FSBtnClick
    end
    object HD0EjectBtn: TButton
      Left = 308
      Top = 50
      Width = 41
      Height = 23
      Caption = 'Eject'
      TabOrder = 5
      OnClick = HD0EjectBtnClick
    end
    object HD0C: TEdit
      Left = 56
      Top = 80
      Width = 41
      Height = 21
      Enabled = False
      TabOrder = 6
      Text = '0'
    end
    object HD0H: TEdit
      Left = 104
      Top = 80
      Width = 41
      Height = 21
      Enabled = False
      TabOrder = 7
      Text = '0'
    end
    object HD0HUD: TUpDown
      Left = 146
      Top = 80
      Width = 25
      Height = 21
      Min = 0
      Orientation = udHorizontal
      Position = 0
      TabOrder = 8
      Wrap = False
      OnClick = HD0HUDClick
    end
    object HD0S: TEdit
      Left = 176
      Top = 80
      Width = 41
      Height = 21
      Enabled = False
      TabOrder = 9
      Text = '0'
    end
    object HD0SUD: TUpDown
      Left = 218
      Top = 80
      Width = 25
      Height = 21
      Min = 0
      Orientation = udHorizontal
      Position = 0
      TabOrder = 10
      Wrap = False
      OnClick = HD0HUDClick
    end
    object HD0ReadOnly: TCheckBox
      Left = 246
      Top = 24
      Width = 73
      Height = 17
      Caption = 'Read Only'
      TabOrder = 1
      OnClick = HD0ReadOnlyClick
    end
    object HD1ReadOnly: TCheckBox
      Left = 246
      Top = 112
      Width = 77
      Height = 17
      Caption = 'Read Only'
      TabOrder = 12
      OnClick = HD1ReadOnlyClick
    end
    object HD0NewBtn: TButton
      Left = 272
      Top = 50
      Width = 33
      Height = 23
      Caption = 'New'
      TabOrder = 4
      OnClick = HD0NewBtnClick
    end
    object HD1NewBtn: TButton
      Left = 272
      Top = 138
      Width = 33
      Height = 23
      Caption = 'New'
      TabOrder = 15
      OnClick = HD1NewBtnClick
    end
  end
  object MicrodriveGroup: TGroupBox
    Left = 8
    Top = 328
    Width = 358
    Height = 217
    Caption = 'Microdrives'
    TabOrder = 2
    object MDV0Label: TLabel
      Left = 11
      Top = 20
      Width = 37
      Height = 13
      Alignment = taRightJustify
      Caption = 'Drive 1:'
    end
    object MDV1Label: TLabel
      Left = 11
      Top = 44
      Width = 37
      Height = 13
      Alignment = taRightJustify
      Caption = 'Drive 2:'
    end
    object MDV3Label: TLabel
      Left = 11
      Top = 92
      Width = 37
      Height = 13
      Alignment = taRightJustify
      Caption = 'Drive 4:'
    end
    object MDV2Label: TLabel
      Left = 11
      Top = 68
      Width = 37
      Height = 13
      Alignment = taRightJustify
      Caption = 'Drive 3:'
    end
    object MDV4Label: TLabel
      Left = 11
      Top = 116
      Width = 37
      Height = 13
      Alignment = taRightJustify
      Caption = 'Drive 5:'
    end
    object MDV5Label: TLabel
      Left = 11
      Top = 140
      Width = 37
      Height = 13
      Alignment = taRightJustify
      Caption = 'Drive 6:'
    end
    object MDV6Label: TLabel
      Left = 11
      Top = 164
      Width = 37
      Height = 13
      Alignment = taRightJustify
      Caption = 'Drive 7:'
    end
    object MDV7Label: TLabel
      Left = 11
      Top = 188
      Width = 37
      Height = 13
      Alignment = taRightJustify
      Caption = 'Drive 8:'
    end
    object MDV0Text: TEdit
      Left = 56
      Top = 16
      Width = 185
      Height = 21
      Enabled = False
      ParentShowHint = False
      ReadOnly = True
      ShowHint = True
      TabOrder = 0
      Text = '< Empty >'
    end
    object MDV0FSBtn: TButton
      Left = 244
      Top = 16
      Width = 25
      Height = 23
      Caption = '...'
      TabOrder = 1
      OnClick = MDVFSBtnClick
    end
    object MDV0EjectBtn: TButton
      Left = 308
      Top = 16
      Width = 41
      Height = 23
      Caption = 'Eject'
      TabOrder = 3
      OnClick = MDVEjectBtnClick
    end
    object MDV1EjectBtn: TButton
      Left = 308
      Top = 40
      Width = 41
      Height = 23
      Caption = 'Eject'
      TabOrder = 7
      OnClick = MDVEjectBtnClick
    end
    object MDV1FSBtn: TButton
      Left = 244
      Top = 40
      Width = 25
      Height = 23
      Caption = '...'
      TabOrder = 5
      OnClick = MDVFSBtnClick
    end
    object MDV1Text: TEdit
      Left = 56
      Top = 40
      Width = 185
      Height = 21
      Enabled = False
      ReadOnly = True
      TabOrder = 4
      Text = '< Empty >'
    end
    object MDV2Text: TEdit
      Left = 56
      Top = 64
      Width = 185
      Height = 21
      Enabled = False
      ParentShowHint = False
      ReadOnly = True
      ShowHint = True
      TabOrder = 8
      Text = '< Empty >'
    end
    object MDV3Text: TEdit
      Left = 56
      Top = 88
      Width = 185
      Height = 21
      Enabled = False
      ReadOnly = True
      TabOrder = 12
      Text = '< Empty >'
    end
    object MDV3FSBtn: TButton
      Left = 244
      Top = 88
      Width = 25
      Height = 23
      Caption = '...'
      TabOrder = 13
      OnClick = MDVFSBtnClick
    end
    object MDV2FSBtn: TButton
      Left = 244
      Top = 64
      Width = 25
      Height = 23
      Caption = '...'
      TabOrder = 9
      OnClick = MDVFSBtnClick
    end
    object MDV2EjectBtn: TButton
      Left = 308
      Top = 64
      Width = 41
      Height = 23
      Caption = 'Eject'
      TabOrder = 11
      OnClick = MDVEjectBtnClick
    end
    object MDV3EjectBtn: TButton
      Left = 308
      Top = 88
      Width = 41
      Height = 23
      Caption = 'Eject'
      TabOrder = 15
      OnClick = MDVEjectBtnClick
    end
    object MDV7Text: TEdit
      Left = 56
      Top = 184
      Width = 185
      Height = 21
      Enabled = False
      ReadOnly = True
      TabOrder = 28
      Text = '< Empty >'
    end
    object MDV6Text: TEdit
      Left = 56
      Top = 160
      Width = 185
      Height = 21
      Enabled = False
      ParentShowHint = False
      ReadOnly = True
      ShowHint = True
      TabOrder = 24
      Text = '< Empty >'
    end
    object MDV5Text: TEdit
      Left = 56
      Top = 136
      Width = 185
      Height = 21
      Enabled = False
      ReadOnly = True
      TabOrder = 20
      Text = '< Empty >'
    end
    object MDV4Text: TEdit
      Left = 56
      Top = 112
      Width = 185
      Height = 21
      Enabled = False
      ParentShowHint = False
      ReadOnly = True
      ShowHint = True
      TabOrder = 16
      Text = '< Empty >'
    end
    object MDV4FSBtn: TButton
      Left = 244
      Top = 112
      Width = 25
      Height = 23
      Caption = '...'
      TabOrder = 17
      OnClick = MDVFSBtnClick
    end
    object MDV5FSBtn: TButton
      Left = 244
      Top = 136
      Width = 25
      Height = 23
      Caption = '...'
      TabOrder = 21
      OnClick = MDVFSBtnClick
    end
    object MDV6FSBtn: TButton
      Left = 244
      Top = 160
      Width = 25
      Height = 23
      Caption = '...'
      TabOrder = 25
      OnClick = MDVFSBtnClick
    end
    object MDV7FSBtn: TButton
      Left = 244
      Top = 184
      Width = 25
      Height = 23
      Caption = '...'
      TabOrder = 29
      OnClick = MDVFSBtnClick
    end
    object MDV7EjectBtn: TButton
      Left = 308
      Top = 184
      Width = 41
      Height = 23
      Caption = 'Eject'
      TabOrder = 31
      OnClick = MDVEjectBtnClick
    end
    object MDV6EjectBtn: TButton
      Left = 308
      Top = 160
      Width = 41
      Height = 23
      Caption = 'Eject'
      TabOrder = 27
      OnClick = MDVEjectBtnClick
    end
    object MDV5EjectBtn: TButton
      Left = 308
      Top = 136
      Width = 41
      Height = 23
      Caption = 'Eject'
      TabOrder = 23
      OnClick = MDVEjectBtnClick
    end
    object MDV4EjectBtn: TButton
      Left = 308
      Top = 112
      Width = 41
      Height = 23
      Caption = 'Eject'
      TabOrder = 19
      OnClick = MDVEjectBtnClick
    end
    object MDV0NewBtn: TButton
      Left = 272
      Top = 16
      Width = 33
      Height = 23
      Caption = 'New'
      TabOrder = 2
      OnClick = MDVNewBtnClick
    end
    object MDV1NewBtn: TButton
      Left = 272
      Top = 40
      Width = 33
      Height = 23
      Caption = 'New'
      TabOrder = 6
      OnClick = MDVNewBtnClick
    end
    object MDV2NewBtn: TButton
      Left = 272
      Top = 64
      Width = 33
      Height = 23
      Caption = 'New'
      TabOrder = 10
      OnClick = MDVNewBtnClick
    end
    object MDV3NewBtn: TButton
      Left = 272
      Top = 88
      Width = 33
      Height = 23
      Caption = 'New'
      TabOrder = 14
      OnClick = MDVNewBtnClick
    end
    object MDV4NewBtn: TButton
      Left = 272
      Top = 112
      Width = 33
      Height = 23
      Caption = 'New'
      TabOrder = 18
      OnClick = MDVNewBtnClick
    end
    object MDV5NewBtn: TButton
      Left = 272
      Top = 136
      Width = 33
      Height = 23
      Caption = 'New'
      TabOrder = 22
      OnClick = MDVNewBtnClick
    end
    object MDV6NewBtn: TButton
      Left = 272
      Top = 160
      Width = 33
      Height = 23
      Caption = 'New'
      TabOrder = 26
      OnClick = MDVNewBtnClick
    end
    object MDV7NewBtn: TButton
      Left = 272
      Top = 184
      Width = 33
      Height = 23
      Caption = 'New'
      TabOrder = 30
      OnClick = MDVNewBtnClick
    end
  end
  object OpenDialogFloppyDiskImage: TOpenDialog
    Filter = 
      'Disk Images|*.dsk;*.mgt;*.img;*.opd;*.opu;*.trd;*.lar;*.zip|+3 D' +
      'isk Images (*.dsk)|*.dsk|Plus D/Disciple Disk Images (*.mgt, *.i' +
      'mg)|*.mgt;*.img|Opus Discovery Disk Images (*.opd, *.opu)|*.opd;' +
      '*.opu|TR-DOS Disk Images (*.trd)|*.trd|Larken Disk Images (*.lar' +
      ')|*.lar|Compressed Disk Images (*.zip)|*.zip'
    Options = [ofOverwritePrompt, ofHideReadOnly, ofPathMustExist, ofCreatePrompt, ofEnableSizing]
    Title = 'Select Floppy Disk Image'
    Left = 40
    Top = 552
  end
  object OpenDialogHardDriveImage: TOpenDialog
    DefaultExt = '.hdf'
    Filter = 
      'All Hard Drive Images (*.hdf; *.vhd)|*.hdf;*.vhd|ZX Hard Drive I' +
      'mages (*.hdf)|*.hdf|VirtualPC Uncompressed Images (*.vhd)|*.vhd'
    Options = [ofOverwritePrompt, ofHideReadOnly, ofPathMustExist, ofEnableSizing]
    Title = 'Select Hard Drive Image'
    Left = 112
    Top = 552
  end
  object SaveDialogNewFloppyDisk: TSaveDialog
    Options = [ofHideReadOnly, ofPathMustExist, ofEnableSizing]
    Left = 8
    Top = 552
  end
  object SaveDialogNewHardDriveImage: TSaveDialog
    DefaultExt = '.hdf'
    Filter = 'ZX Hard Drive Images (*.hdf)|*.hdf'
    Options = [ofHideReadOnly, ofPathMustExist, ofEnableSizing]
    Title = 'Create Hard Drive Image'
    Left = 80
    Top = 552
  end
end
