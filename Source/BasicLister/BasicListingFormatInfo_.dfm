object BasicListingFormatInfoForm: TBasicListingFormatInfoForm
  Left = 196
  Top = 155
  BorderIcons = [biSystemMenu]
  BorderStyle = bsToolWindow
  Caption = 'Saving BASIC Listing As Text'
  ClientHeight = 338
  ClientWidth = 418
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  Icon.Data = {
    0000010002002020100000000000E80200002600000010101000000000002801
    00000E0300002800000020000000400000000100040000000000800200000000
    0000000000000000000000000000000000000000800000800000008080008000
    0000800080008080000080808000C0C0C0000000FF0000FF000000FFFF00FF00
    0000FF00FF00FFFF0000FFFFFF00000000000000000000000000000000007777
    7777777777777777777777777777000000000000000000000000000000000000
    0000000000000000000000000000000070070070070070070070070070000000
    7007007007007007007007007000000000000000000000000000000000000000
    0700700700700700700700700700000007007007007007007007007007000000
    0000000000000000000000000000000070070070070070070070070070000000
    7007007007007007007007007000000000000000000000000000000000000007
    0070070070070070070070070000000700700700700700700700700700000000
    0000000000000000000000000000000000000000000000000000000000000777
    7777777777777777777777777770000000000000000000000000000000000099
    9919091999090000000000000000000900019109090900000000000000000000
    9001910191190000000000000000009999190919990900000000000000000000
    0000000000000000000000000000000000000000000000000000000000000000
    0000000000000000000000000000000000000000000000000000000000000770
    7077777777700000000000000000077777777070777700000000000000000000
    0000000000000000000000000000000000000000000000000000000000000000
    0000000000000000000000000000FFFFFFFF0000000000000000000000000000
    0000000000000000000000000000000000000000000000000000000000000000
    0000000000000000000000000000000000000000000000000000000000000000
    0000000000000000000000000000000000000000000000000000000000000000
    000000000000FFFFFFFFFFFFFFFF280000001000000020000000010004000000
    0000C00000000000000000000000000000000000000000000000000080000080
    00000080800080000000800080008080000080808000C0C0C0000000FF0000FF
    000000FFFF00FF000000FF00FF00FFFF0000FFFFFF0077777777777777770000
    000000000000088878878878878000F7FF7FF7FF7FF00088788788788780F7FF
    7FF7FF7FF0000000000000000000077777777777777000000000000000000999
    0999900000000090909090000000099909999000000000000000000000000000
    0000000000007777777700000000000000000000000000000000000000000000
    0000000000000000000000000000000000000000000000000000000000000000
    000000000000000000000000000000000000FFFF0000}
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  object PageControl: TPageControl
    Left = 8
    Top = 8
    Width = 401
    Height = 289
    ActivePage = TabSheet1
    TabOrder = 0
    object TabSheet1: TTabSheet
      Caption = 'ZX80'
      object Label1: TLabel
        Left = 8
        Top = 8
        Width = 372
        Height = 52
        Caption = 
          'Inverse characters and graphic symbols are encoded in accordance' +
          ' with the scheme used by the ZXText2P utility. In addition, non-' +
          'displayable character codes are output as \nn where nn is the ch' +
          'aracter code expressed as a 2 digit hexadecimal number.'
        WordWrap = True
      end
      object Label2: TLabel
        Left = 8
        Top = 64
        Width = 374
        Height = 13
        Caption = 
          'Space suppression between consecutive tokens is replicated in th' +
          'e text output.'
        WordWrap = True
      end
      object Label3: TLabel
        Left = 8
        Top = 81
        Width = 376
        Height = 39
        Caption = 
          'The content of a REM statement is by default output as text if a' +
          'll characters are displayable, and as \nn character codes if any' +
          ' non-displayable characters exist based upon the assumption that' +
          ' the REM holds machine code. A configuration'
        WordWrap = True
      end
      object Label4: TLabel
        Left = 8
        Top = 152
        Width = 377
        Height = 39
        Caption = 
          'The pound symbol is by default output as £, but this is not part' +
          ' of the ASCII character set (it has code $A3). A configuration o' +
          'ption allows it to be output in \nn character code format instea' +
          'd, i.e. \0C. '
        WordWrap = True
      end
      object Label5: TLabel
        Left = 8
        Top = 200
        Width = 255
        Height = 13
        Caption = 'ZX80 BASIC listing files are saved with extension B80.'
        WordWrap = True
      end
      object Label17: TLabel
        Left = 8
        Top = 120
        Width = 366
        Height = 26
        Caption = 
          'A configuration option allows the contents to be output in \nn c' +
          'haracter code format irrespective of whether all characters are ' +
          'displayable.'
        WordWrap = True
      end
      object Label19: TLabel
        Left = 8
        Top = 222
        Width = 285
        Height = 13
        Caption = 'Details of the ZXText2P encoding format can be found here:'
      end
      object Weblink1: TLabel
        Left = 8
        Top = 238
        Width = 219
        Height = 13
        Caption = 'http://freestuff.grok.co.uk/zxtext2p/index.html'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        OnClick = Weblink1Click
      end
    end
    object TabSheet2: TTabSheet
      Caption = 'ZX81'
      ImageIndex = 1
      object Label6: TLabel
        Left = 8
        Top = 8
        Width = 372
        Height = 52
        Caption = 
          'Inverse characters and graphic symbols are encoded in accordance' +
          ' with the scheme used by the ZXText2P utility. In addition, non-' +
          'displayable character codes are output as \nn where nn is the ch' +
          'aracter code expressed as a 2 digit hexadecimal number.'
        WordWrap = True
      end
      object Label7: TLabel
        Left = 8
        Top = 64
        Width = 374
        Height = 13
        Caption = 
          'Space suppression between consecutive tokens is replicated in th' +
          'e text output.'
        WordWrap = True
      end
      object Label8: TLabel
        Left = 8
        Top = 81
        Width = 376
        Height = 39
        Caption = 
          'The content of a REM statement is by default output as text if a' +
          'll characters are displayable, and as \nn character codes if any' +
          ' non-displayable characters exist based upon the assumption that' +
          ' the REM holds machine code. A configuration'
        WordWrap = True
      end
      object Label9: TLabel
        Left = 8
        Top = 152
        Width = 377
        Height = 39
        Caption = 
          'The pound symbol is by default output as £, but this is not part' +
          ' of the ASCII character set (it has code $A3). A configuration o' +
          'ption allows it to be output in \nn character code format instea' +
          'd, i.e. \0C. '
        WordWrap = True
      end
      object Label10: TLabel
        Left = 8
        Top = 200
        Width = 255
        Height = 13
        Caption = 'ZX81 BASIC listing files are saved with extension B81.'
        WordWrap = True
      end
      object Label16: TLabel
        Left = 8
        Top = 120
        Width = 366
        Height = 26
        Caption = 
          'A configuration option allows the contents to be output in \nn c' +
          'haracter code format irrespective of whether all characters are ' +
          'displayable.'
        WordWrap = True
      end
      object Label21: TLabel
        Left = 8
        Top = 222
        Width = 285
        Height = 13
        Caption = 'Details of the ZXText2P encoding format can be found here:'
      end
      object Weblink2: TLabel
        Left = 8
        Top = 238
        Width = 219
        Height = 13
        Caption = 'http://freestuff.grok.co.uk/zxtext2p/index.html'
        OnClick = Weblink2Click
      end
    end
    object TabSheet3: TTabSheet
      Caption = 'ZX Spectrum'
      ImageIndex = 2
      object Label11: TLabel
        Left = 8
        Top = 8
        Width = 377
        Height = 52
        Caption = 
          'Graphic symbols are encoded in a similar manner to the scheme us' +
          'ed by the ZXText2P utility but encodings begin with ¬ (code $AC)' +
          ' instead of \. In addition, UDGs are output as ¬_n where n is th' +
          'e UDG letter, and non-displayable'
        WordWrap = True
      end
      object Label12: TLabel
        Left = 8
        Top = 80
        Width = 374
        Height = 13
        Caption = 
          'Space suppression between consecutive tokens is replicated in th' +
          'e text output.'
        WordWrap = True
      end
      object Label13: TLabel
        Left = 8
        Top = 99
        Width = 377
        Height = 39
        Caption = 
          'The pound symbol is by default output as £, but this is not part' +
          ' of the ASCII character set (it has code $A3). A configuration o' +
          'ption allows it to be output in ¬nn character code format instea' +
          'd, i.e. \60.'
        WordWrap = True
      end
      object Label14: TLabel
        Left = 8
        Top = 143
        Width = 377
        Height = 39
        Caption = 
          'The pound symbol is by default output as ©, but this is not part' +
          ' of the ASCII character set (it has code $A9). A configuration o' +
          'ption allows it to be output in ¬nn character code format instea' +
          'd, i.e. \7F.'
        WordWrap = True
      end
      object Label15: TLabel
        Left = 8
        Top = 188
        Width = 321
        Height = 13
        Caption = 'ZX Spectrum BASIC listing files are saved with extension B82.'
        WordWrap = True
      end
      object Label18: TLabel
        Left = 8
        Top = 48
        Width = 367
        Height = 26
        Caption = 
          'character codes are output as ¬nn where nn is the character code' +
          ' expressed as a 2 digit hexadecimal number.'
        WordWrap = True
      end
      object Label23: TLabel
        Left = 8
        Top = 208
        Width = 285
        Height = 13
        Caption = 'Details of the ZXText2P encoding format can be found here:'
      end
      object Weblink3: TLabel
        Left = 8
        Top = 224
        Width = 219
        Height = 13
        Caption = 'http://freestuff.grok.co.uk/zxtext2p/index.html'
        OnClick = Weblink3Click
      end
    end
  end
  object btnOK: TButton
    Left = 176
    Top = 304
    Width = 68
    Height = 25
    Caption = 'OK'
    Default = True
    ModalResult = 1
    TabOrder = 1
  end
end
