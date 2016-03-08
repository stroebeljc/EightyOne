object Dbg: TDbg
  Left = 499
  Top = 108
  BorderStyle = bsToolWindow
  Caption = 'Debug Window'
  ClientHeight = 304
  ClientWidth = 493
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
  object GroupBox1: TGroupBox
    Left = 0
    Top = 0
    Width = 305
    Height = 137
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
      Top = 24
      Width = 25
      Height = 13
      Alignment = taRightJustify
      AutoSize = False
      Caption = 'HL:'
    end
    object Label2: TLabel
      Left = 3
      Top = 40
      Width = 25
      Height = 13
      Alignment = taRightJustify
      AutoSize = False
      Caption = 'BC:'
    end
    object Label3: TLabel
      Left = 3
      Top = 56
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
    end
    object BC: TLabel
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
    end
    object DE: TLabel
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
    end
    object F: TLabel
      Left = 32
      Top = 88
      Width = 48
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
    end
    object Label6: TLabel
      Left = 110
      Top = 24
      Width = 30
      Height = 13
      Alignment = taRightJustify
      AutoSize = False
      Caption = 'HL'#39':'
    end
    object HL_: TLabel
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
    end
    object BC_: TLabel
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
    end
    object DE_: TLabel
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
    end
    object F_: TLabel
      Left = 144
      Top = 88
      Width = 48
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
      Top = 56
      Width = 30
      Height = 13
      Alignment = taRightJustify
      AutoSize = False
      Caption = 'DE'#39':'
    end
    object Label15: TLabel
      Left = 110
      Top = 40
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
    end
    object Label7: TLabel
      Left = 222
      Top = 112
      Width = 23
      Height = 13
      AutoSize = False
      Caption = 'IM:'
    end
    object IM: TLabel
      Left = 248
      Top = 112
      Width = 17
      Height = 13
      AutoSize = False
      Caption = '0'
    end
    object Label9: TLabel
      Left = 70
      Top = 112
      Width = 73
      Height = 13
      AutoSize = False
      Caption = 'Interupts:'
    end
    object Interupts: TLabel
      Left = 144
      Top = 112
      Width = 56
      Height = 14
      Caption = 'Disabled'
    end
    object Label11: TLabel
      Left = 4
      Top = 112
      Width = 37
      Height = 13
      AutoSize = False
      Caption = 'Halt:'
    end
    object Halt: TLabel
      Left = 40
      Top = 112
      Width = 25
      Height = 13
      AutoSize = False
      Caption = 'Yes'
    end
  end
  object GroupBox2: TGroupBox
    Left = 0
    Top = 136
    Width = 225
    Height = 167
    Caption = 'Program'
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Courier New'
    Font.Style = []
    ParentFont = False
    TabOrder = 1
    object Label8: TLabel
      Left = 8
      Top = 16
      Width = 21
      Height = 14
      Caption = 'PC:'
    end
    object Disass0: TLabel
      Left = 8
      Top = 32
      Width = 196
      Height = 14
      Caption = '0000 xxxxxxxx LD HL,(IX+$2f)'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Pitch = fpFixed
      Font.Style = []
      ParentFont = False
    end
    object Disass1: TLabel
      Left = 8
      Top = 48
      Width = 196
      Height = 14
      Caption = '0000 xxxxxxxx LD HL,(IX+$2f)'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Pitch = fpFixed
      Font.Style = []
      ParentFont = False
    end
    object Disass2: TLabel
      Left = 8
      Top = 64
      Width = 196
      Height = 14
      Caption = '0000 xxxxxxxx LD HL,(IX+$2f)'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Pitch = fpFixed
      Font.Style = []
      ParentFont = False
    end
    object Disass3: TLabel
      Left = 8
      Top = 80
      Width = 196
      Height = 14
      Caption = '0000 xxxxxxxx LD HL,(IX+$2f)'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Pitch = fpFixed
      Font.Style = []
      ParentFont = False
    end
    object Disass4: TLabel
      Left = 8
      Top = 96
      Width = 196
      Height = 14
      Caption = '0000 xxxxxxxx LD HL,(IX+$2f)'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Pitch = fpFixed
      Font.Style = []
      ParentFont = False
    end
    object Disass5: TLabel
      Left = 8
      Top = 112
      Width = 196
      Height = 14
      Caption = '0000 xxxxxxxx LD HL,(IX+$2f)'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Pitch = fpFixed
      Font.Style = []
      ParentFont = False
    end
    object Disass6: TLabel
      Left = 8
      Top = 128
      Width = 196
      Height = 14
      Caption = '0000 xxxxxxxx LD HL,(IX+$2f)'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Pitch = fpFixed
      Font.Style = []
      ParentFont = False
    end
    object Disass7: TLabel
      Left = 8
      Top = 144
      Width = 196
      Height = 14
      Caption = '0000 xxxxxxxx LD HL,(IX+$2f)'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Pitch = fpFixed
      Font.Style = []
      ParentFont = False
    end
  end
  object GroupBox3: TGroupBox
    Left = 312
    Top = 0
    Width = 177
    Height = 137
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
    end
  end
  object GroupBox4: TGroupBox
    Left = 312
    Top = 136
    Width = 177
    Height = 167
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
    object Label26: TLabel
      Left = 114
      Top = 32
      Width = 34
      Height = 13
      Caption = 'Display'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
    end
    object RunStop: TButton
      Left = 96
      Top = 136
      Width = 75
      Height = 25
      Caption = 'Stop'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      TabOrder = 0
      OnClick = RunStopClick
    end
    object SingleStep: TButton
      Left = 96
      Top = 88
      Width = 75
      Height = 25
      Caption = 'Single Step'
      Enabled = False
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      TabOrder = 1
      OnClick = SingleStepClick
    end
    object StepOver: TButton
      Left = 96
      Top = 112
      Width = 75
      Height = 25
      Caption = 'Step Over'
      Enabled = False
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      TabOrder = 2
      OnClick = StepOverClick
    end
    object Continuous: TCheckBox
      Left = 96
      Top = 16
      Width = 73
      Height = 17
      Caption = 'Continuous'
      Checked = True
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      State = cbChecked
      TabOrder = 3
      OnClick = ContinuousClick
    end
    object AddBrkBtn: TButton
      Left = 8
      Top = 144
      Width = 41
      Height = 17
      Caption = 'Add'
      TabOrder = 4
      OnClick = AddBrkBtnClick
    end
    object DelBrkBtn: TButton
      Left = 48
      Top = 144
      Width = 41
      Height = 17
      Caption = 'Del'
      TabOrder = 5
      OnClick = DelBrkBtnClick
    end
    object BPList: TStringGrid
      Left = 8
      Top = 32
      Width = 81
      Height = 105
      ColCount = 1
      DefaultRowHeight = 14
      FixedCols = 0
      RowCount = 1
      FixedRows = 0
      Options = [goFixedVertLine, goFixedHorzLine, goVertLine, goRowSelect]
      ScrollBars = ssVertical
      TabOrder = 6
      ColWidths = (
        77)
    end
    object SkipNMIBtn: TCheckBox
      Left = 96
      Top = 48
      Width = 73
      Height = 17
      Caption = 'Skip NMI'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      TabOrder = 7
      OnClick = SkipNMIBtnClick
    end
    object SkipINTBtn: TCheckBox
      Left = 96
      Top = 64
      Width = 73
      Height = 17
      Caption = 'Skip INT'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      TabOrder = 8
      OnClick = SkipINTBtnClick
    end
  end
  object GroupBox5: TGroupBox
    Left = 232
    Top = 136
    Width = 73
    Height = 167
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
    end
    object Label17: TLabel
      Left = 19
      Top = 16
      Width = 21
      Height = 14
      Caption = 'SP:'
    end
  end
end
