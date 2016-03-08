object Dbg: TDbg
  Left = 292
  Top = 117
  BorderIcons = [biSystemMenu]
  BorderStyle = bsToolWindow
  Caption = 'Debug Window'
  ClientHeight = 406
  ClientWidth = 575
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnClose = FormClose
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object SymRom: TLabel
    Left = 464
    Top = 181
    Width = 25
    Height = 13
    Caption = 'ROM'
  end
  object SymApp: TLabel
    Left = 496
    Top = 181
    Width = 21
    Height = 13
    Caption = 'APP'
  end
  object GroupBox1: TGroupBox
    Left = 2
    Top = 0
    Width = 305
    Height = 201
    Caption = 'Z80'
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Courier New'
    Font.Pitch = fpFixed
    Font.Style = []
    ParentFont = False
    TabOrder = 0
    object Label1: TLabel
      Left = 3
      Top = 56
      Width = 25
      Height = 13
      Alignment = taRightJustify
      AutoSize = False
      Caption = 'HL:'
    end
    object Label2: TLabel
      Left = 3
      Top = 24
      Width = 25
      Height = 13
      Alignment = taRightJustify
      AutoSize = False
      Caption = 'BC:'
    end
    object Label3: TLabel
      Left = 3
      Top = 40
      Width = 25
      Height = 13
      Alignment = taRightJustify
      AutoSize = False
      Caption = 'DE:'
    end
    object Label4: TLabel
      Left = 3
      Top = 72
      Width = 25
      Height = 13
      Alignment = taRightJustify
      AutoSize = False
      Caption = 'A:'
    end
    object Label5: TLabel
      Left = 3
      Top = 88
      Width = 25
      Height = 13
      Alignment = taRightJustify
      AutoSize = False
      Caption = 'F:'
    end
    object HL: TLabel
      Left = 32
      Top = 56
      Width = 40
      Height = 13
      AutoSize = False
      Caption = '$FFFF'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Pitch = fpFixed
      Font.Style = []
      ParentFont = False
      PopupMenu = MemDumpPopup
      OnClick = HLClick
      OnMouseDown = HLMouseDown
    end
    object BC: TLabel
      Left = 32
      Top = 24
      Width = 40
      Height = 13
      AutoSize = False
      Caption = '$FFFF'
      Color = clBtnFace
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Pitch = fpFixed
      Font.Style = []
      ParentColor = False
      ParentFont = False
      ParentShowHint = False
      PopupMenu = MemDumpPopup
      ShowHint = False
      OnClick = BCClick
      OnMouseDown = BCMouseDown
    end
    object DE: TLabel
      Left = 32
      Top = 40
      Width = 40
      Height = 13
      AutoSize = False
      Caption = '$FFFF'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Pitch = fpFixed
      Font.Style = []
      ParentFont = False
      PopupMenu = MemDumpPopup
      OnClick = DEClick
      OnMouseDown = DEMouseDown
    end
    object A: TLabel
      Left = 32
      Top = 72
      Width = 40
      Height = 13
      AutoSize = False
      Caption = '$FF'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Pitch = fpFixed
      Font.Style = []
      ParentFont = False
      OnClick = AClick
    end
    object F: TLabel
      Left = 32
      Top = 88
      Width = 57
      Height = 13
      AutoSize = False
      Caption = '00000000'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Pitch = fpFixed
      Font.Style = []
      ParentFont = False
      OnClick = FClick
    end
    object Label6: TLabel
      Left = 110
      Top = 56
      Width = 30
      Height = 13
      Alignment = taRightJustify
      AutoSize = False
      Caption = 'HL'#39':'
    end
    object HL_: TLabel
      Left = 144
      Top = 56
      Width = 40
      Height = 13
      AutoSize = False
      Caption = '$FFFF'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Pitch = fpFixed
      Font.Style = []
      ParentFont = False
      PopupMenu = MemDumpPopup
      OnClick = HL_Click
      OnMouseDown = HL_MouseDown
    end
    object BC_: TLabel
      Left = 144
      Top = 24
      Width = 40
      Height = 13
      AutoSize = False
      Caption = '$FFFF'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Pitch = fpFixed
      Font.Style = []
      ParentFont = False
      PopupMenu = MemDumpPopup
      OnClick = BC_Click
      OnMouseDown = BC_MouseDown
    end
    object DE_: TLabel
      Left = 144
      Top = 40
      Width = 40
      Height = 13
      AutoSize = False
      Caption = '$FFFF'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Pitch = fpFixed
      Font.Style = []
      ParentFont = False
      PopupMenu = MemDumpPopup
      OnClick = DE_Click
      OnMouseDown = DE_MouseDown
    end
    object A_: TLabel
      Left = 144
      Top = 72
      Width = 40
      Height = 13
      AutoSize = False
      Caption = '$FF'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Pitch = fpFixed
      Font.Style = []
      ParentFont = False
      OnClick = A_Click
    end
    object F_: TLabel
      Left = 144
      Top = 88
      Width = 57
      Height = 13
      AutoSize = False
      Caption = '00000000'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Pitch = fpFixed
      Font.Style = []
      ParentFont = False
      OnClick = F_Click
    end
    object Label12: TLabel
      Left = 110
      Top = 88
      Width = 30
      Height = 13
      Alignment = taRightJustify
      AutoSize = False
      Caption = 'F'#39':'
    end
    object Label13: TLabel
      Left = 110
      Top = 72
      Width = 30
      Height = 13
      Alignment = taRightJustify
      AutoSize = False
      Caption = 'A'#39':'
    end
    object Label14: TLabel
      Left = 110
      Top = 40
      Width = 30
      Height = 13
      Alignment = taRightJustify
      AutoSize = False
      Caption = 'DE'#39':'
    end
    object Label15: TLabel
      Left = 110
      Top = 24
      Width = 30
      Height = 13
      Alignment = taRightJustify
      AutoSize = False
      Caption = 'BC'#39':'
    end
    object Label16: TLabel
      Left = 224
      Top = 24
      Width = 20
      Height = 13
      Alignment = taRightJustify
      AutoSize = False
      Caption = 'IX:'
    end
    object IX: TLabel
      Left = 248
      Top = 24
      Width = 40
      Height = 13
      AutoSize = False
      Caption = '$FFFF'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Pitch = fpFixed
      Font.Style = []
      ParentFont = False
      PopupMenu = MemDumpPopup
      OnClick = IXClick
      OnMouseDown = IXMouseDown
    end
    object Label18: TLabel
      Left = 224
      Top = 40
      Width = 20
      Height = 13
      Alignment = taRightJustify
      AutoSize = False
      Caption = 'IY:'
    end
    object IY: TLabel
      Left = 248
      Top = 40
      Width = 40
      Height = 13
      AutoSize = False
      Caption = '$FFFF'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Pitch = fpFixed
      Font.Style = []
      ParentFont = False
      PopupMenu = MemDumpPopup
      OnClick = IYClick
      OnMouseDown = IYMouseDown
    end
    object Label20: TLabel
      Left = 224
      Top = 56
      Width = 20
      Height = 13
      Alignment = taRightJustify
      AutoSize = False
      Caption = 'IR:'
    end
    object IR: TLabel
      Left = 248
      Top = 56
      Width = 40
      Height = 13
      AutoSize = False
      Caption = '$FFFF'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Pitch = fpFixed
      Font.Style = []
      ParentFont = False
      PopupMenu = MemDumpPopup
      OnClick = IRClick
      OnMouseDown = IRMouseDown
    end
    object Label22: TLabel
      Left = 224
      Top = 72
      Width = 20
      Height = 13
      Alignment = taRightJustify
      AutoSize = False
      Caption = 'SP:'
    end
    object SP: TLabel
      Left = 248
      Top = 72
      Width = 40
      Height = 13
      AutoSize = False
      Caption = '$FFFF'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Pitch = fpFixed
      Font.Style = []
      ParentFont = False
      PopupMenu = MemDumpPopup
      OnClick = SPClick
      OnMouseDown = SPMouseDown
    end
    object Label24: TLabel
      Left = 224
      Top = 88
      Width = 20
      Height = 13
      Alignment = taRightJustify
      AutoSize = False
      Caption = 'PC:'
    end
    object PC: TLabel
      Left = 248
      Top = 88
      Width = 40
      Height = 13
      AutoSize = False
      Caption = '$FFFF'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Pitch = fpFixed
      Font.Style = []
      ParentFont = False
      PopupMenu = MemDumpPopup
      OnClick = PCClick
      OnMouseDown = PCMouseDown
    end
    object Label7: TLabel
      Left = 254
      Top = 123
      Width = 23
      Height = 13
      AutoSize = False
      Caption = 'IM:'
    end
    object IM: TLabel
      Left = 280
      Top = 123
      Width = 17
      Height = 13
      AutoSize = False
      Caption = '0'
      OnClick = IMClick
    end
    object Label9: TLabel
      Left = 96
      Top = 123
      Width = 73
      Height = 13
      AutoSize = False
      Caption = 'Interupts:'
    end
    object Interupts: TLabel
      Left = 170
      Top = 123
      Width = 56
      Height = 14
      Caption = 'Disabled'
      OnClick = InteruptsClick
    end
    object Label11: TLabel
      Left = 8
      Top = 123
      Width = 37
      Height = 13
      AutoSize = False
      Caption = 'Halt:'
    end
    object Halt: TLabel
      Left = 48
      Top = 123
      Width = 25
      Height = 13
      AutoSize = False
      Caption = 'Yes'
    end
    object Label28: TLabel
      Left = 32
      Top = 102
      Width = 56
      Height = 14
      Caption = 'SZ-H-VNC'
      Font.Charset = ANSI_CHARSET
      Font.Color = clGrayText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Pitch = fpFixed
      Font.Style = []
      ParentFont = False
    end
    object Label29: TLabel
      Left = 144
      Top = 102
      Width = 56
      Height = 14
      Caption = 'SZ-H-VNC'
      Font.Charset = ANSI_CHARSET
      Font.Color = clGrayText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Pitch = fpFixed
      Font.Style = []
      ParentFont = False
    end
    object SingleStep: TButton
      Left = 8
      Top = 154
      Width = 65
      Height = 25
      Caption = 'Single Step'
      Enabled = False
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Pitch = fpFixed
      Font.Style = []
      ParentFont = False
      TabOrder = 0
      OnClick = SingleStepClick
    end
    object StepOver: TButton
      Left = 80
      Top = 154
      Width = 65
      Height = 25
      Caption = 'Step Over'
      Enabled = False
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Pitch = fpFixed
      Font.Style = []
      ParentFont = False
      TabOrder = 1
      OnClick = StepOverClick
    end
    object RunStop: TButton
      Left = 152
      Top = 154
      Width = 65
      Height = 25
      Caption = 'Stop'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Pitch = fpFixed
      Font.Style = []
      ParentFont = False
      TabOrder = 2
      OnClick = RunStopClick
    end
    object Continuous: TCheckBox
      Left = 224
      Top = 144
      Width = 73
      Height = 17
      Caption = 'Continuous'
      Checked = True
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Pitch = fpFixed
      Font.Style = []
      ParentFont = False
      State = cbChecked
      TabOrder = 3
      OnClick = ContinuousClick
    end
    object SkipNMIBtn: TCheckBox
      Left = 224
      Top = 160
      Width = 73
      Height = 17
      Caption = 'Skip NMI'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Pitch = fpFixed
      Font.Style = []
      ParentFont = False
      TabOrder = 4
      OnClick = SkipNMIBtnClick
    end
    object SkipINTBtn: TCheckBox
      Left = 224
      Top = 176
      Width = 73
      Height = 17
      Caption = 'Skip INT'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Pitch = fpFixed
      Font.Style = []
      ParentFont = False
      TabOrder = 5
      OnClick = SkipINTBtnClick
    end
  end
  object GroupBox2: TGroupBox
    Left = 2
    Top = 202
    Width = 383
    Height = 199
    Caption = 'Program'
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Courier New'
    Font.Style = []
    ParentFont = False
    TabOrder = 1
    object Disass0: TLabel
      Left = 16
      Top = 32
      Width = 301
      Height = 14
      Caption = 'LABEL000    $0000  xxxxxxxx  LD HL,(IX+$zz)'
      Color = clBtnFace
      Font.Charset = ANSI_CHARSET
      Font.Color = clGrayText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Pitch = fpFixed
      Font.Style = []
      ParentColor = False
      ParentFont = False
    end
    object Disass1: TLabel
      Left = 16
      Top = 48
      Width = 70
      Height = 14
      Caption = '<history2>'
      Color = clBtnFace
      Font.Charset = ANSI_CHARSET
      Font.Color = clGrayText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Pitch = fpFixed
      Font.Style = []
      ParentColor = False
      ParentFont = False
    end
    object Disass2: TLabel
      Left = 16
      Top = 64
      Width = 70
      Height = 14
      Caption = '<history3>'
      Color = clBtnFace
      Font.Charset = ANSI_CHARSET
      Font.Color = clGrayText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Pitch = fpFixed
      Font.Style = []
      ParentColor = False
      ParentFont = False
    end
    object Disass3: TLabel
      Left = 16
      Top = 80
      Width = 350
      Height = 14
      Caption = '<currentPC> 01234567890123456789012345678901234567'
      Color = clHighlightText
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Pitch = fpFixed
      Font.Style = []
      ParentColor = False
      ParentFont = False
    end
    object Disass4: TLabel
      Left = 16
      Top = 96
      Width = 7
      Height = 14
      Caption = '"'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Pitch = fpFixed
      Font.Style = []
      ParentFont = False
    end
    object Disass5: TLabel
      Left = 16
      Top = 112
      Width = 7
      Height = 14
      Caption = '"'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Pitch = fpFixed
      Font.Style = []
      ParentFont = False
    end
    object Disass6: TLabel
      Left = 16
      Top = 128
      Width = 7
      Height = 14
      Caption = '"'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Pitch = fpFixed
      Font.Style = []
      ParentFont = False
    end
    object Disass7: TLabel
      Left = 16
      Top = 144
      Width = 7
      Height = 14
      Caption = '"'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Pitch = fpFixed
      Font.Style = []
      ParentFont = False
    end
    object Disass8: TLabel
      Left = 16
      Top = 160
      Width = 7
      Height = 14
      Caption = '"'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Pitch = fpFixed
      Font.Style = []
      ParentFont = False
    end
    object Disass9: TLabel
      Left = 16
      Top = 176
      Width = 7
      Height = 14
      Caption = '"'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Pitch = fpFixed
      Font.Style = []
      ParentFont = False
    end
    object Label8: TLabel
      Left = 16
      Top = 16
      Width = 259
      Height = 14
      Caption = 'Label       Addr   Code      Mnemonic'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Pitch = fpFixed
      Font.Style = []
      ParentFont = False
    end
  end
  object GroupBoxZX81: TGroupBox
    Left = 394
    Top = 234
    Width = 177
    Height = 167
    Caption = 'ZX81'
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Courier New'
    Font.Style = []
    ParentFont = False
    TabOrder = 2
    object Label39: TLabel
      Left = 5
      Top = 24
      Width = 105
      Height = 14
      Alignment = taRightJustify
      AutoSize = False
      Caption = 'NMI Gen:'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = []
      ParentFont = False
    end
    object Label10: TLabel
      Left = 5
      Top = 40
      Width = 105
      Height = 14
      Alignment = taRightJustify
      AutoSize = False
      Caption = 'HSYNC Gen:'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = []
      ParentFont = False
    end
    object Label19: TLabel
      Left = 5
      Top = 56
      Width = 105
      Height = 14
      Alignment = taRightJustify
      AutoSize = False
      Caption = 'Scanline:'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = []
      ParentFont = False
    end
    object Label21: TLabel
      Left = 5
      Top = 72
      Width = 105
      Height = 14
      Alignment = taRightJustify
      AutoSize = False
      Caption = 'Row Counter:'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = []
      ParentFont = False
    end
    object Label23: TLabel
      Left = 5
      Top = 88
      Width = 105
      Height = 14
      Alignment = taRightJustify
      Caption = 'Shift Register:'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = []
      ParentFont = False
    end
    object NMIGen: TLabel
      Left = 115
      Top = 24
      Width = 57
      Height = 14
      AutoSize = False
      Caption = 'OFF'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = []
      ParentFont = False
      OnClick = NMIGenClick
    end
    object Scanline: TLabel
      Left = 115
      Top = 56
      Width = 57
      Height = 14
      AutoSize = False
      Caption = '299'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = []
      ParentFont = False
    end
    object RowCount: TLabel
      Left = 115
      Top = 72
      Width = 57
      Height = 13
      AutoSize = False
      Caption = '7'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = []
      ParentFont = False
    end
    object ShiftReg: TLabel
      Left = 115
      Top = 88
      Width = 57
      Height = 13
      AutoSize = False
      Caption = '00000000'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = []
      ParentFont = False
    end
    object HSYNCGen: TLabel
      Left = 115
      Top = 40
      Width = 57
      Height = 14
      AutoSize = False
      Caption = 'OFF'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = []
      ParentFont = False
      OnClick = HSYNCGenClick
    end
    object Label27: TLabel
      Left = 54
      Top = 104
      Width = 56
      Height = 14
      Alignment = taRightJustify
      Caption = 'TStates:'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = []
      ParentFont = False
    end
    object TStates: TLabel
      Left = 115
      Top = 104
      Width = 57
      Height = 13
      AutoSize = False
      Caption = '00000000'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = []
      ParentFont = False
    end
  end
  object GroupBox4: TGroupBox
    Left = 394
    Top = 0
    Width = 177
    Height = 169
    Caption = 'Control'
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Courier New'
    Font.Style = []
    ParentFont = False
    TabOrder = 3
    object Label25: TLabel
      Left = 8
      Top = 16
      Width = 59
      Height = 13
      Caption = 'Breakpoints:'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
    end
    object AddBrkBtn: TButton
      Left = 120
      Top = 104
      Width = 49
      Height = 25
      Caption = 'Add'
      TabOrder = 0
      OnClick = AddBrkBtnClick
    end
    object DelBrkBtn: TButton
      Left = 120
      Top = 136
      Width = 49
      Height = 25
      Caption = 'Del'
      TabOrder = 1
      OnClick = DelBrkBtnClick
    end
    object BPList: TStringGrid
      Left = 8
      Top = 32
      Width = 105
      Height = 129
      ColCount = 1
      DefaultColWidth = 10
      DefaultRowHeight = 14
      FixedCols = 0
      RowCount = 1
      FixedRows = 0
      Options = [goFixedVertLine, goFixedHorzLine, goVertLine, goRowSelect]
      ScrollBars = ssVertical
      TabOrder = 2
    end
  end
  object GroupBox5: TGroupBox
    Left = 314
    Top = 0
    Width = 73
    Height = 201
    Caption = 'Stack'
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Courier New'
    Font.Style = []
    ParentFont = False
    TabOrder = 4
    object Stack0: TLabel
      Left = 19
      Top = 32
      Width = 35
      Height = 14
      Caption = '$FFFF'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Pitch = fpFixed
      Font.Style = []
      ParentFont = False
      PopupMenu = MemDumpPopup
      OnClick = Stack0Click
      OnMouseDown = Stack0MouseDown
    end
    object Stack1: TLabel
      Left = 19
      Top = 48
      Width = 35
      Height = 14
      Caption = '$FFFF'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = []
      ParentFont = False
      PopupMenu = MemDumpPopup
      OnClick = Stack0Click
      OnMouseDown = Stack0MouseDown
    end
    object Stack2: TLabel
      Left = 19
      Top = 64
      Width = 35
      Height = 14
      Caption = '$FFFF'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = []
      ParentFont = False
      PopupMenu = MemDumpPopup
      OnClick = Stack0Click
      OnMouseDown = Stack0MouseDown
    end
    object Stack3: TLabel
      Left = 19
      Top = 80
      Width = 35
      Height = 14
      Caption = '$FFFF'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = []
      ParentFont = False
      PopupMenu = MemDumpPopup
      OnClick = Stack0Click
      OnMouseDown = Stack0MouseDown
    end
    object Stack4: TLabel
      Left = 19
      Top = 96
      Width = 35
      Height = 14
      Caption = '$FFFF'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = []
      ParentFont = False
      PopupMenu = MemDumpPopup
      OnClick = Stack0Click
      OnMouseDown = Stack0MouseDown
    end
    object Stack5: TLabel
      Left = 19
      Top = 112
      Width = 35
      Height = 14
      Caption = '$FFFF'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = []
      ParentFont = False
      PopupMenu = MemDumpPopup
      OnClick = Stack0Click
      OnMouseDown = Stack0MouseDown
    end
    object Stack6: TLabel
      Left = 19
      Top = 128
      Width = 35
      Height = 14
      Caption = '$FFFF'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = []
      ParentFont = False
      PopupMenu = MemDumpPopup
      OnClick = Stack0Click
      OnMouseDown = Stack0MouseDown
    end
    object Stack7: TLabel
      Left = 19
      Top = 144
      Width = 35
      Height = 14
      Caption = '$FFFF'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = []
      ParentFont = False
      PopupMenu = MemDumpPopup
      OnClick = Stack0Click
      OnMouseDown = Stack0MouseDown
    end
    object Label17: TLabel
      Left = 19
      Top = 16
      Width = 21
      Height = 14
      Caption = 'SP:'
    end
  end
  object GroupBoxAce: TGroupBox
    Left = 392
    Top = 234
    Width = 179
    Height = 167
    Caption = 'Jupiter Ace'
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Courier New'
    Font.Style = []
    ParentFont = False
    TabOrder = 5
    object AceStk0: TLabel
      Left = 43
      Top = 32
      Width = 35
      Height = 14
      Caption = '$FFFF'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Pitch = fpFixed
      Font.Style = []
      ParentFont = False
      OnClick = AceStk0Click
    end
    object AceStk1: TLabel
      Left = 43
      Top = 48
      Width = 35
      Height = 14
      Caption = '$FFFF'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = []
      ParentFont = False
      OnClick = AceStk0Click
    end
    object AceStk2: TLabel
      Left = 43
      Top = 64
      Width = 35
      Height = 14
      Caption = '$FFFF'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = []
      ParentFont = False
      OnClick = AceStk0Click
    end
    object AceStk3: TLabel
      Left = 43
      Top = 80
      Width = 35
      Height = 14
      Caption = '$FFFF'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = []
      ParentFont = False
      OnClick = AceStk0Click
    end
    object AceStk4: TLabel
      Left = 43
      Top = 96
      Width = 35
      Height = 14
      Caption = '$FFFF'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = []
      ParentFont = False
      OnClick = AceStk0Click
    end
    object AceStk5: TLabel
      Left = 43
      Top = 112
      Width = 35
      Height = 14
      Caption = '$FFFF'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = []
      ParentFont = False
      OnClick = AceStk0Click
    end
    object AceStk6: TLabel
      Left = 43
      Top = 128
      Width = 35
      Height = 14
      Caption = '$FFFF'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = []
      ParentFont = False
      OnClick = AceStk0Click
    end
    object AceStk7: TLabel
      Left = 43
      Top = 144
      Width = 35
      Height = 14
      Caption = '$FFFF'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = []
      ParentFont = False
      OnClick = AceStk0Click
    end
    object Label36: TLabel
      Left = 8
      Top = 16
      Width = 84
      Height = 14
      Caption = 'Forth Stack:'
    end
    object Label37: TLabel
      Left = 83
      Top = 32
      Width = 7
      Height = 14
      Caption = ':'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Pitch = fpFixed
      Font.Style = []
      ParentFont = False
      OnClick = Stack0Click
    end
    object Label38: TLabel
      Left = 83
      Top = 48
      Width = 7
      Height = 14
      Caption = ':'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = []
      ParentFont = False
    end
    object Label40: TLabel
      Left = 83
      Top = 64
      Width = 7
      Height = 14
      Caption = ':'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = []
      ParentFont = False
    end
    object Label41: TLabel
      Left = 83
      Top = 80
      Width = 7
      Height = 14
      Caption = ':'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = []
      ParentFont = False
    end
    object Label42: TLabel
      Left = 83
      Top = 96
      Width = 7
      Height = 14
      Caption = ':'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = []
      ParentFont = False
    end
    object Label43: TLabel
      Left = 83
      Top = 112
      Width = 7
      Height = 14
      Caption = ':'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = []
      ParentFont = False
    end
    object Label44: TLabel
      Left = 83
      Top = 128
      Width = 7
      Height = 14
      Caption = ':'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = []
      ParentFont = False
    end
    object Label45: TLabel
      Left = 83
      Top = 144
      Width = 7
      Height = 14
      Caption = ':'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = []
      ParentFont = False
    end
    object AceStkVal0: TLabel
      Left = 99
      Top = 32
      Width = 35
      Height = 14
      Caption = '$FFFF'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Pitch = fpFixed
      Font.Style = []
      ParentFont = False
      OnClick = AceStkVal0Click
    end
    object AceStkVal1: TLabel
      Left = 99
      Top = 48
      Width = 35
      Height = 14
      Caption = '$FFFF'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = []
      ParentFont = False
      OnClick = AceStkVal0Click
    end
    object AceStkVal2: TLabel
      Left = 99
      Top = 64
      Width = 35
      Height = 14
      Caption = '$FFFF'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = []
      ParentFont = False
      OnClick = AceStkVal0Click
    end
    object AceStkVal3: TLabel
      Left = 99
      Top = 80
      Width = 35
      Height = 14
      Caption = '$FFFF'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = []
      ParentFont = False
      OnClick = AceStkVal0Click
    end
    object AceStkVal4: TLabel
      Left = 99
      Top = 96
      Width = 35
      Height = 14
      Caption = '$FFFF'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = []
      ParentFont = False
      OnClick = AceStkVal0Click
    end
    object AceStkVal5: TLabel
      Left = 99
      Top = 112
      Width = 35
      Height = 14
      Caption = '$FFFF'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = []
      ParentFont = False
      OnClick = AceStkVal0Click
    end
    object AceStkVal6: TLabel
      Left = 99
      Top = 128
      Width = 35
      Height = 14
      Caption = '$FFFF'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = []
      ParentFont = False
      OnClick = AceStkVal0Click
    end
    object AceStkVal7: TLabel
      Left = 99
      Top = 144
      Width = 35
      Height = 14
      Caption = '$FFFF'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = []
      ParentFont = False
      OnClick = AceStkVal0Click
    end
  end
  object EnableHistory: TCheckBox
    Left = 464
    Top = 212
    Width = 65
    Height = 17
    Caption = 'Enable'
    TabOrder = 6
  end
  object History: TButton
    Left = 392
    Top = 212
    Width = 65
    Height = 17
    Caption = 'History'
    TabOrder = 7
    OnClick = HistoryClick
  end
  object Symbols: TButton
    Left = 392
    Top = 180
    Width = 65
    Height = 17
    Caption = 'Symbols'
    TabOrder = 8
    OnClick = SymbolsClick
  end
  object MemDumpPopup: TPopupMenu
    Left = 536
    Top = 184
    object MemDumpFromHere1: TMenuItem
      Caption = 'Show Memory'
      OnClick = MemDumpFromHere1Click
    end
    object AddBreak1: TMenuItem
      Caption = 'Add Breakpoint'
      object OnExecute1: TMenuItem
        Caption = 'On Execute'
        OnClick = AddBreak1Click
      end
      object OnRead1: TMenuItem
        Tag = 1
        Caption = 'On Read'
        OnClick = AddBreak1Click
      end
      object OnWrite1: TMenuItem
        Tag = 2
        Caption = 'On Write'
        OnClick = AddBreak1Click
      end
    end
  end
end
