/****************************************************************************
 *
 * Copyright (c) 2016 CRI Middleware Co., Ltd.
 *
 ****************************************************************************/

using UnityEngine;
using UnityEngine.EventSystems;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using System.Linq;

// ���ӁF
// SampleIO�� CRIWARE SDK for Unity �T���v���v���W�F�N�g�p�̃��[�e�B���e�B�N���X�ł��B
// �{�N���X�� CRIWARE �v���O�C���̋@�\�Ƃ͖��֌W�ł��B�v���O�C���̎g�����ɂ��Ă�
// SDK�����̃}�j���A�����Q�Ƃ��ĉ������B�܂��A�{�N���X�̎����͎��ۂ̃A�v���P�[�V�����J����
// �Q�l�ɂ͂ł��܂���BCRIWARE SDK �T���v���̓���m�F�ȊO�̗p�r�Ŏg�p���Ȃ��ł��������B
public class SampleIO : MonoBehaviour
{
    // SampleIO����ē��͂����L�[�̎��ʎq�B�Q�[���p�b�h�̃L�[�ɑΉ������Ďg��
    [System.Flags]
    public enum Key
    {
        Noinput = 0x0,
        Up = 0x1,
        Left = 0x2,
        Right = 0x4,
        Down = 0x8,
        Select = 0x10,
        Start = 0x20,
        A = 0x40,
        B = 0x80,
        C = 0x100,
        D = 0x200,
        E = 0x400,
        F = 0x800,
        Invalid
    }


    // SampleIO��������o�̓C�x���g�B��{�I�ɂ͂P�t���[���ɂ��P�C�x���g�����������
    // ���ӁF�P�t���[�����ɕ������s���ꂽ���o�͂́A�܂Ƃ߂ĂP�C�x���g�Ƃ��ĕ\������
    sealed private class EventInfo
    {
        public int frameCount;
        public Key inputKey;
        public List<string> outputContentList;

        
        public EventInfo() {
            outputContentList = new List<string>();
        }

        
        public EventInfo(int frame_count, Key input_key, List<string> content_list) {
            frameCount = frame_count;
            inputKey = input_key;
            outputContentList = content_list;
        }
    }


    static private class EventInfoInterface
    {
        // �ŐV�t���[���̓��o�̓C�x���g�ɉ��l�����邩�ǂ����`�F�b�N����
        // ���ӁFSampleIO�͑S�Ă̓��o�̓C�x���g���L�^����킯�ł͂Ȃ�
        public static bool IsCurrentFrameEventInfoWorthy(EventInfo event_info_cur, EventInfo event_info_prev) {
            // �O��t���[���ƈقȂ���͂�����Ή��l�A��
            if (event_info_cur.inputKey != Key.Noinput) {
                return true;
            }

            // �O��t���[���ƈقȂ�o�͂�����Ή��l�A��
            if (event_info_cur.outputContentList.Count > 0) {
                if (event_info_cur.outputContentList.Count != event_info_prev.outputContentList.Count) {
                    return true;
                }
                for (int i = 0; i < event_info_cur.outputContentList.Count; i++) {
                    if (event_info_cur.outputContentList[i] != event_info_prev.outputContentList[i]) {
                        return true;
                    }
                }
            }

            return false;
        }


        public static void ClearEventInfo(EventInfo event_info) {
			event_info.frameCount = Time.frameCount - singleton.frameCountWhenRecordStarted;
            event_info.inputKey = Key.Noinput;
            event_info.outputContentList.Clear();
        }


        // �ŐV���o�̓C�x���g�̃I�u�W�F�N�g���A�O����o�̓C�x���g�̃I�u�W�F�N�g�Ɠ���ւ���̂Ɏg��
        public static void SwapEventInfo(ref EventInfo left, ref EventInfo right) {
            var tmp = left;
            left = right;
            right = tmp;
        }


        // �w�肵�����o�̓C�x���g�̓��e�𕶎���ɐ��`
        public static string Format(EventInfo event_info) {
            string record_entry = "";
            event_info.outputContentList.ForEach(delegate(string c)
            {
                record_entry = record_entry + c + ", ";
            });
            record_entry = System.String.Format("{0}, {1}, {2}",
                    event_info.frameCount,
                    event_info.inputKey,
                    record_entry);
            return record_entry;
        }
    }

    static private SampleIO singleton = null;
    private EventInfo currentFrameEvent = new EventInfo();
    private EventInfo prevFrameEvent = new EventInfo();
	private int frameCountWhenRecordStarted;
    private int frameCountWhenReplayStarted;

	private Camera virtualPadCamera = null;
	private Canvas canvas = null;

    [SerializeField]
    private bool dontDestroy = false;
    public bool DontDestroy {
        get { return dontDestroy; }
    }

	[SerializeField]
	private bool verbose = true;
	static public bool Verbose {
		get { return singleton.verbose; }
	}

	[SerializeField]
	private bool showRecordingControls = true;
	static public bool ShowRecordingControls {
		get { return singleton.showRecordingControls; }
	}

	[SerializeField]
	private bool showVirtualPad = false;
	static public bool ShowVirtualPad {
		get { return singleton.showVirtualPad; }
	}
	
    // recordMode: ���t���[���̓��o�̓C�x���g�����R�[�h�t�@�C���ɋL�^���郂�[�h
    //[SerializeField]
    private bool recordMode = true;
    static public bool RecordMode {
        get { return singleton.recordMode; }
        private set { singleton.recordMode = value; }
    }
    private StreamWriter recordFileWriter = null;
    static private bool isRecording = false;
    static private bool finishRecordingRequested = false;
	static private bool beginReplayRequested = false;


    // replayMode: ���v���C�t�@�C��������̓C�x���g���Č����郂�[�h
    // ���ӁFrecordMode�ō쐬�������R�[�h�t�@�C�������v���C�t�@�C���Ƃ��Ďg�����Ƃ��ł���
    //[SerializeField]
    private bool replayMode = true;
    static public bool ReplayMode {
        get { return singleton.replayMode; }
    }
    private Stack<LinkedList<EventInfo>> replayEventListStack = new Stack<LinkedList<EventInfo>>();
    
	static private bool isReplaying = false;
	static public bool IsReplaying { 
		get { return isReplaying; }
	}

	[SerializeField]
	private string recordFilePath = null;
	static public string RecordFilePath {
		get { return singleton.recordFilePath; }
	}

	[SerializeField]
	private bool beginRecordWithScene = false;
	static public bool BeginRecordWithScene {
		get { return singleton.beginRecordWithScene; }
	}

	[SerializeField]
	private bool beginReplayWithScene = false;
	static public bool BeginReplayWithScene {
		get { return singleton.beginReplayWithScene; }
	}

    // UI�Ƃ��ĉ�ʏo�͂��郉�x��
    private string[] labels = new string[20];
    static public string[] Labels {
        get { return singleton.labels; }
    }


    static public void SetLabel(int index, string content) {
        if (singleton == null) {
            Debug.LogError("SetLabel Failed. SampleIO instance is null.");
            return;
        }
        singleton.labels[index] = content;
    }


    static public void ClearLabels() {
        if (singleton == null) {
            Debug.LogError("ClearLabels Failed. SampleIO instance is null.");
            return;
        }
        for (int i = 0; i < singleton.labels.Length; i++) {
            singleton.labels[i] = "";
        }        
    }

    // SampleIO���������
    static public void PushKey(int inputKey) {
        if (singleton == null) {
            Debug.LogError("PushKey Failed. SampleIO instance is null.");
            return;
        }
        // ����: �P�t���[�����ɉ����ꂽ�L�[�̏��Ԃ܂ł͊o�����Ȃ�
        singleton.currentFrameEvent.inputKey = (Key)inputKey;
    }


    // �A�v���P�[�V�����̃V�[�����W�b�N����Ă΂��
    static public Key GetPushedKey() {
        if (singleton == null) {
            Debug.LogError("GetPushedKey Failed. SampleIO instance is null.");
            return Key.Invalid;
        }
        return singleton.currentFrameEvent.inputKey;
    }


    // SampleIO������o�́B�A�v���P�[�V�����̃V�[�����W�b�N����Ă΂��
    static public void PrintLog(string content) {
        if (singleton == null) {
            Debug.LogError("PrintLog Failed. SampleIO instance is null.");
            return;
        }
		if (singleton.verbose) {
			singleton.currentFrameEvent.outputContentList.Add(content);
		}
    }


    // �O������replayEventList���X�V����
    static public bool BeginReplay(string filePath) {
        if (singleton == null) {
            Debug.LogError("BeginReplay Failed. SampleIO instance is null.");
            return false;
        }
        if (singleton.replayMode) {
            if (isReplaying) {
                Debug.LogWarning("BeginReplay Failed. Replay is already started.");
                return false;
            }

			// case of stop recording then immediatly replaying
			if (finishRecordingRequested) {
				beginReplayRequested = true;
				return true;
			}
			beginReplayRequested = false;

			filePath = string.IsNullOrEmpty(filePath) ? singleton.recordFilePath : filePath;
			filePath = singleton.GetAbsoluteFilePathForReplay(filePath);
			singleton.PushReplayEventListToStack(singleton.replayEventListStack, filePath);
            if (singleton.verbose) {
                Debug.Log(System.String.Format("Begin replay from {0}", filePath));
            }
            singleton.frameCountWhenReplayStarted = Time.frameCount;
            isReplaying = true;
        }
        return singleton.replayMode;
    }


    static public bool FinishReplay() {
        if (singleton == null) {
            Debug.LogError("FinishReplay Failed. SampleIO instance is null.");
            return false;
        }
        if (singleton.replayMode) {
            if (singleton.replayEventListStack.Count > 0) {
                singleton.replayEventListStack.Pop();
            }
            if (singleton.verbose) {
                Debug.Log(System.String.Format("Finish replay"));
            }
            isReplaying = false;
        }
        return singleton.replayMode;    
    }


    static public bool BeginRecording(string filePath) {
        if (singleton == null) {
            Debug.LogError("BeginRecording Failed. SampleIO instance is null.");
            return false;
        }
        if (singleton.recordMode) {
            if (isRecording) {
                Debug.LogWarning("BeginRecording Failed. Recording is already started.");
                return false;
            }
			filePath = string.IsNullOrEmpty(filePath) ? singleton.recordFilePath : filePath;
			filePath = singleton.GetAbsoluteFilePathForRecord(filePath);
            singleton.recordFileWriter = new StreamWriter(filePath);
            if (singleton.recordFileWriter != null) {
                if (singleton.verbose) {
                    Debug.Log(System.String.Format("Begin recording onto {0}", filePath));
                }
				singleton.frameCountWhenRecordStarted = Time.frameCount;
				isRecording = true;
            } else {
                Debug.LogError("BeginRecording Failed. recordFileWriter is null.");
                return false;
            }
        }
        return singleton.recordMode;    
    }


    static private void finishRecording() {
        if (finishRecordingRequested == false) {
            return;
        }
        if (singleton.recordFileWriter == null) {
			finishRecordingRequested = false;
            return;
        }
        // StreamWriter��Close(or Flush)���Ȃ��ƃt�@�C���o�͂��s��Ȃ�(����܂ł̓o�b�t�@�ɏ������߂Ă���炵��)
#if !UNITY_EDITOR && UNITY_WINRT
        singleton.recordFileWriter.Flush();
#else
        singleton.recordFileWriter.Close();
#endif
        if (singleton.verbose) {
            Debug.Log(System.String.Format("Finished recording"));
        }
        singleton.recordFileWriter = null;
        isRecording = false;
        finishRecordingRequested = false;
		if (beginReplayRequested) {
			BeginReplay(null);
		}
    }

    static public bool FinishRecording() {
        if (singleton == null) {
            Debug.LogError("FinishRecording Failed. SampleIO instance is null.");
            return false;
        }
		if (singleton.recordFileWriter == null) {
			return false;
		}
		if (singleton.recordMode) {
            finishRecordingRequested = true;
            //finishRecording();
        }
        return singleton.recordMode;
    }

    
    private void PushReplayEventListToStack(Stack<LinkedList<EventInfo>> event_list_stack, string file_path) {
        LinkedList<EventInfo> newEventList = new LinkedList<EventInfo>();

        // ���v���C�t�@�C���̓��e�����X�g�Ƃ��ă��[�h
        using (StreamReader sr = new StreamReader(file_path)) {
            while (sr.EndOfStream == false) {
                var entries = sr.ReadLine().Split(',');
                Key input_key = (Key)System.Enum.Parse(typeof(Key), entries[1]);
                newEventList.AddLast(new EventInfo(int.Parse(entries[0]), input_key, new List<string>() { "" }));
            }
        }
        event_list_stack.Push(newEventList);
    }


	// UI-level API: handler if replay button is pressed.
	static public void OnPushReplayButton(bool enable) {
		if (singleton == null) {
			Debug.LogError("OnPushReplayKey Failed. SampleIO instance is null.");
			return;
		}
		if (singleton.replayMode == false) {
			Debug.LogError("OnPushReplayKey Failed. ReplayMode must be enabled.");
			return;
		}
		if (string.IsNullOrEmpty(singleton.recordFilePath)) {
			Debug.LogError("OnPushReplayKey Failed. Please choose a file path to record.");
			return;
		}
		if (enable) { 
			if (!isReplaying) {
				if (isRecording) {
					FinishRecording();
				}
				BeginReplay(null);
			}
		} else {
			FinishReplay();
		}
	}


	// UI-level API: handler if record button is pressed.
	static public void OnPushRecordButton(bool enable) {
		if (singleton == null) {
			Debug.LogError("OnPushReplayKey Failed. SampleIO instance is null.");
			return;
		}
		if (singleton.recordMode == false) {
			Debug.LogError("OnPushReplayKey Failed. RecordMode must be enabled.");
			return;
		}
		if (enable) { 
			if (!isRecording) {
				if (isReplaying) {
					FinishReplay();
				}
				BeginRecording(null);
			}
		} else {
			FinishRecording();
		}
	}


    void Awake() {
        if (SampleIO.singleton == null) {
            SampleIO.singleton = this;
        } else {
            Debug.LogWarning("SampleIO instance already exists.");
            Destroy(this.gameObject);
        }

#if UNITY_PS4
        StandaloneInputModule inputModule = this.transform.FindChild("EventSystem").GetComponent<StandaloneInputModule>();
        if (inputModule) {
            inputModule.horizontalAxis = "PS4HUDHorizontal";
            inputModule.verticalAxis = "PS4HUDVertical";
            inputModule.submitButton = "PS4HUDSubmit";
            inputModule.cancelButton = "PS4HUDCancel";
        }
#endif

        virtualPadCamera = GameObject.Find("VirtualPadCamera").GetComponent<Camera>();
		canvas = this.GetComponent<Canvas>();

        if (dontDestroy) {
            Object.DontDestroyOnLoad(this.gameObject);
        }

		if (beginRecordWithScene && beginReplayWithScene) {
			Debug.LogError("Both record and replay with scene are enabled. Only Record is performed.");
		}

		if (string.IsNullOrEmpty(recordFilePath)) {
			if (beginRecordWithScene || beginReplayWithScene) {
				Debug.LogError("Record File Path is not set. Automatic Record/Replay cannot be performed.");
			}
		} else {
			if (beginRecordWithScene) {
				string filePath = GetAbsoluteFilePathForRecord(recordFilePath);
				if (File.Exists(filePath) == false) {
					Debug.LogError("Record File Path cannot be found. Automatic Record cannot be performed.");
				} else {
					BeginRecording(filePath);
				}
			} else if (beginReplayWithScene) {
				string filePath = GetAbsoluteFilePathForReplay(recordFilePath);
				if (File.Exists(filePath) == false) {
					Debug.LogError("Record File Path cannot be found. Automatic Replay cannot be performed.");
				} else {
					BeginReplay(filePath);
				}
			}
		}
    }


	string GetAbsoluteFilePathForRecord(string filePath) {
		if (!string.IsNullOrEmpty(filePath) && !Path.IsPathRooted(filePath)) {
#if UNITY_EDITOR
			string replayPath = Path.Combine(Application.streamingAssetsPath, "Replay");
#else
			string replayPath = Path.Combine(Application.persistentDataPath, "Replay");
#endif
			if (Directory.Exists(replayPath) == false) {
				Directory.CreateDirectory(replayPath);
			}
			filePath = Path.Combine(replayPath, filePath);
		}
		return filePath;
	}
	
	string GetAbsoluteFilePathForReplay(string filePath) {
		if (!string.IsNullOrEmpty(filePath) && !Path.IsPathRooted(filePath)) {
#if UNITY_EDITOR
			string replayPath = Path.Combine(Application.streamingAssetsPath, "Replay");
			filePath = Path.Combine(replayPath, filePath);
#else
			string replayPath = Path.Combine(Application.persistentDataPath, "Replay");
			string newFilePath = Path.Combine(replayPath, filePath);
			if (File.Exists(newFilePath) == false) {
				// look to unity assets if not file exist in user persistent path.
				replayPath = Path.Combine(Application.streamingAssetsPath, "Replay");
				return Path.Combine(replayPath, filePath);
			}
			return newFilePath;
#endif
		}
		return filePath;
	}

    
    void Update() {
       	// Debug.Log(Time.frameCount);

		// Set the camera a always good plane distance - if perspective camera used
		// adj = opp / tan(alpha)
		if (virtualPadCamera.orthographic == false) {
			RectTransform rectTransform = this.GetComponent<RectTransform>();
			float opp = rectTransform.rect.height / 2.0f;
			float alpha = virtualPadCamera.fieldOfView / 2.0f;
			float dist = opp / Mathf.Tan (alpha * Mathf.Deg2Rad);
			Vector3 plane = canvas.transform.position;
			Vector3 cam = virtualPadCamera.transform.localPosition;
			virtualPadCamera.transform.localPosition = new Vector3 (cam.x, cam.y, plane.z - dist);
		}
    }

    void LateUpdate() {
        // ���l�̂�����o�̓C�x���g�������L�^
        if (EventInfoInterface.IsCurrentFrameEventInfoWorthy(currentFrameEvent, prevFrameEvent)) {
            var record_entry = EventInfoInterface.Format(currentFrameEvent);
            if (recordMode) {
                if (recordFileWriter != null) {
                    recordFileWriter.WriteLine(record_entry);
                }
            }
            if (verbose) {
                Debug.Log("[LOG] " + record_entry);
            }
        }

        EventInfoInterface.SwapEventInfo(ref prevFrameEvent, ref currentFrameEvent);
        EventInfoInterface.ClearEventInfo(currentFrameEvent);
        //Debug.Log(Time.frameCount + "'");

        if (replayMode) {
			if (this.replayEventListStack.Count > 0) {
				// ���̓C�x���g���Č�
				if ((replayEventListStack.Peek().First != null) && (replayEventListStack.Peek().First.Value.frameCount <= Time.frameCount - frameCountWhenReplayStarted)) {
					PushKey((int)replayEventListStack.Peek().First.Value.inputKey);
					replayEventListStack.Peek().RemoveFirst();
					if (replayEventListStack.Peek().Count == 0) {
						FinishReplay();
					}
				}
			}
        }

        finishRecording();
    }

	void OnGUI() {



		// Render VirtualPad in front of any layer!
		if (Event.current.type == EventType.Repaint){
			virtualPadCamera.Render();
		}
	}

	
    void OnDestroy() {
        if (SampleIO.singleton == this) {
            // �V�[������OnDestroy��FinishRecording/Replay���Ă��A����SampleIO�C���X�^���X�͔j������Ă��邩������Ȃ�
            // ������FinishRecording/Replay���Ă����Ȃ��ƁA�����ƃt�@�C���o�͎���̏I���������s���Ȃ��댯��������
			finishRecordingRequested = true;
            finishRecording();
            FinishReplay();
            SampleIO.singleton = null;
        }
    }
}
