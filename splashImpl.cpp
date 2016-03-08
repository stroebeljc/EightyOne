                 spl->SetProgress(33,"");
                 spl->IncProgress("Main Timer"); Application->CreateForm(__classid(TForm1), &Form1);

                 if (ShowSplash)
                 {
                        spl->Show();
                        Application->ProcessMessages();
                 }

                 spl->IncProgress("Symbol Browser"); Application->CreateForm(__classid(TSymbolBrowser), &SymbolBrowser);
                 spl->IncProgress("EditVal"); Application->CreateForm(__classid(TEditValue), &EditValue);
                 spl->IncProgress("About Box"); Application->CreateForm(__classid(TAbout), &About);
                 spl->IncProgress("Keyboard"); Application->CreateForm(__classid(TKeyboard), &Keyboard);
                 spl->IncProgress("Speed"); Application->CreateForm(__classid(TSpeed), &Speed);
                 spl->IncProgress("Keyboard Helper"); Application->CreateForm(__classid(TKb), &Kb);
                 spl->IncProgress("Wav Loader"); Application->CreateForm(__classid(TWavLoad), &WavLoad);
                 spl->IncProgress("Debugger"); Application->CreateForm(__classid(TDbg), &Dbg);
                 spl->IncProgress("Debugger"); Application->CreateForm(__classid(TMemoryWindow), &MemoryWindow);
                 spl->IncProgress("ZX Printer emulator"); Application->CreateForm(__classid(TPrinter), &Printer);
                 spl->IncProgress("TV emulation"); Application->CreateForm(__classid(TArtifacts), &Artifacts);
                 spl->IncProgress("Sound"); Application->CreateForm(__classid(TSoundOutput), &SoundOutput);
                 spl->IncProgress("CPU History"); Application->CreateForm(__classid(THistoryBox), &HistoryBox);
                 spl->IncProgress("Memory Manager"); Application->CreateForm(__classid(TMemSave), &MemSave);
                 spl->IncProgress("zx97"); Application->CreateForm(__classid(TZX97Dialog), &ZX97Dialog);
                 spl->IncProgress("Serial Port"); Application->CreateForm(__classid(TSerialConfig), &SerialConfig);
                 spl->IncProgress("Tzx"); Application->CreateForm(__classid(TTZX), &TZX);
                 spl->IncProgress("Tzx"); Application->CreateForm(__classid(TEditPauseForm), &EditPauseForm);
                 spl->IncProgress("Tzx"); Application->CreateForm(__classid(TEditArchiveInfo), &EditArchiveInfo);
                 spl->IncProgress("Tzx"); Application->CreateForm(__classid(TEditTextForm), &EditTextForm);
                 spl->IncProgress("Tzx"); Application->CreateForm(__classid(TEditHWInfoForm), &EditHWInfoForm);
                 spl->IncProgress("Tzx"); Application->CreateForm(__classid(TEditDataForm), &EditDataForm);
                 spl->IncProgress("Tzx"); Application->CreateForm(__classid(TFSSettings), &FSSettings);
                 spl->IncProgress("Tzx"); Application->CreateForm(__classid(TEditGeneralForm), &EditGeneralForm);
                 spl->IncProgress("Interface 1"); Application->CreateForm(__classid(TIF1), &IF1);
                 spl->IncProgress("P3 Floppy"); Application->CreateForm(__classid(TP3Drive), &P3Drive);
                 spl->IncProgress("HDF Support"); Application->CreateForm(__classid(TCreateHDF), &CreateHDF);
                 spl->IncProgress("Machine Emulation"); Application->CreateForm(__classid(THW), &HW);
                 spl->IncProgress("Parallel Port"); Application->CreateForm(__classid(TParallelPort), &ParallelPort);
                 spl->IncProgress("Midi"); Application->CreateForm(__classid(TMidiForm), &MidiForm);
                 spl->IncProgress("ZIP Support"); Application->CreateForm(__classid(TZipFile), &ZipFile);
                 spl->IncProgress("Debug68k"); Application->CreateForm(__classid(TDebug68k), &Debug68k);
                 spl->Close();
