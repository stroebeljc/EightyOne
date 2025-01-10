object MidiForm: TMidiForm
  Left = 313
  Top = 236
  BorderIcons = []
  BorderStyle = bsToolWindow
  Caption = 'Sound Settings'
  ClientHeight = 312
  ClientWidth = 312
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  Scaled = False
  OnDestroy = FormDestroy
  PixelsPerInch = 96
  TextHeight = 13
  object GroupBox1: TGroupBox
    Left = 8
    Top = 184
    Width = 297
    Height = 65
    Caption = 'Send MIDI Output to:'
    TabOrder = 1
    object MidiBox: TComboBox
      Left = 16
      Top = 24
      Width = 265
      Height = 21
      Style = csDropDownList
      Anchors = [akLeft, akRight]
      ItemHeight = 13
      TabOrder = 0
      OnChange = MidiChange
    end
  end
  object OK: TButton
    Left = 238
    Top = 278
    Width = 67
    Height = 25
    Anchors = [akRight, akBottom]
    Caption = 'OK'
    TabOrder = 3
    OnClick = OKClick
  end
  object GroupBox2: TGroupBox
    Left = 8
    Top = 8
    Width = 297
    Height = 169
    Caption = 'Mix'
    TabOrder = 0
    object Label1: TLabel
      Left = 32
      Top = 20
      Width = 56
      Height = 13
      Caption = 'AY Channel'
    end
    object Label2: TLabel
      Left = 120
      Top = 39
      Width = 34
      Height = 13
      Caption = 'Beeper'
    end
    object Label3: TLabel
      Left = 24
      Top = 39
      Width = 7
      Height = 13
      Caption = 'A'
    end
    object Label4: TLabel
      Left = 56
      Top = 39
      Width = 7
      Height = 13
      Caption = 'B'
    end
    object Label5: TLabel
      Left = 88
      Top = 39
      Width = 7
      Height = 13
      Caption = 'C'
    end
    object Label6: TLabel
      Left = 64
      Top = 141
      Width = 65
      Height = 13
      Alignment = taCenter
      AutoSize = False
      Caption = '-   Mute   -'
    end
    object Label7: TLabel
      Left = 217
      Top = 20
      Width = 61
      Height = 13
      Caption = 'Stereo Mode'
    end
    object Label8: TLabel
      Left = 168
      Top = 39
      Width = 37
      Height = 13
      Caption = 'Speech'
    end
    object ChAVol: TTrackBar
      Left = 16
      Top = 51
      Width = 41
      Height = 89
      Max = 31
      Orientation = trVertical
      Frequency = 16
      Position = 0
      SelEnd = 0
      SelStart = 0
      TabOrder = 0
      TickMarks = tmBottomRight
      TickStyle = tsAuto
      OnChange = MidiChange
    end
    object ChBVol: TTrackBar
      Left = 48
      Top = 51
      Width = 41
      Height = 89
      Max = 31
      Orientation = trVertical
      Frequency = 16
      Position = 0
      SelEnd = 0
      SelStart = 0
      TabOrder = 1
      TickMarks = tmBottomRight
      TickStyle = tsAuto
      OnChange = MidiChange
    end
    object ChCVol: TTrackBar
      Left = 80
      Top = 51
      Width = 41
      Height = 89
      Max = 31
      Orientation = trVertical
      Frequency = 16
      Position = 0
      SelEnd = 0
      SelStart = 0
      TabOrder = 2
      TickMarks = tmBottomRight
      TickStyle = tsAuto
      OnChange = MidiChange
    end
    object BeeperVol: TTrackBar
      Left = 128
      Top = 51
      Width = 41
      Height = 89
      Max = 31
      Orientation = trVertical
      Frequency = 16
      Position = 0
      SelEnd = 0
      SelStart = 0
      TabOrder = 3
      TickMarks = tmBottomRight
      TickStyle = tsAuto
      OnChange = MidiChange
    end
    object AYMute: TCheckBox
      Left = 52
      Top = 139
      Width = 17
      Height = 17
      TabOrder = 4
      OnClick = MidiChange
    end
    object BeeperMute: TCheckBox
      Left = 131
      Top = 139
      Width = 17
      Height = 17
      TabOrder = 5
      OnClick = MidiChange
    end
    object RadioButtonMono: TRadioButton
      Left = 224
      Top = 41
      Width = 63
      Height = 17
      Caption = 'Mono'
      Checked = True
      TabOrder = 6
      TabStop = True
    end
    object RadioButtonACB: TRadioButton
      Left = 224
      Top = 58
      Width = 63
      Height = 17
      Caption = 'ACB'
      TabOrder = 7
    end
    object SpeechMute: TCheckBox
      Left = 180
      Top = 139
      Width = 17
      Height = 17
      TabOrder = 8
      OnClick = MidiChange
    end
  end
  object BeeperSettingsGroupBox: TGroupBox
    Left = 8
    Top = 256
    Width = 177
    Height = 49
    Caption = 'Beeper Settings'
    TabOrder = 2
    object BeeperExcludeHSyncs: TCheckBox
      Left = 8
      Top = 24
      Width = 161
      Height = 17
      Caption = 'Exclude ZX81 HSync Pulses'
      TabOrder = 0
      OnClick = BeeperExcludeHSyncsClick
    end
  end
  object SpeechVol: TTrackBar
    Left = 184
    Top = 59
    Width = 41
    Height = 89
    Max = 31
    Orientation = trVertical
    Frequency = 16
    Position = 0
    SelEnd = 0
    SelStart = 0
    TabOrder = 4
    TickMarks = tmBottomRight
    TickStyle = tsAuto
    OnChange = MidiChange
  end
end
