/**
 * @file    ui_lang.c
 * @brief   多语言文案表实现（全部为 UTF-8 静态字符串）。
 *
 * @details
 *  - 表顺序必须与 ui_lang.h 的 ui_str_id_t 完全一致；
 *  - 某语言未翻译的条目填 NULL，运行时自动回退英语；
 *  - 新增/修改文案后，运行 tools/font_gen.py 重新生成字库（脚本会自动
 *    收集本文件所有字符串字符），否则新字符无字形。
 */
#include "ui_lang.h"

/* ------------------- English ------------------- */
static const char *ui_str_en[] = {
    "Record",               /* UI_STR_REC */
    "Photo",                /* UI_STR_PHOTO */
    "Timelapse",            /* UI_STR_TIMELAPSE */
    "Playback",             /* UI_STR_PLAYBACK */
    "Settings",             /* UI_STR_SETTINGS */
    "Mode Menu",            /* UI_STR_MODE_MENU */
    "Resolution",           /* UI_STR_RESOLUTION */
    "Loop Record",          /* UI_STR_LOOP_RECORD */
    "WDR",                  /* UI_STR_WDR */
    "Flip",                 /* UI_STR_FLIP */
    "IR Night",             /* UI_STR_IR_NIGHT */
    "Mic",                  /* UI_STR_MIC */
    "Time Stamp",           /* UI_STR_WATERMARK */
    "WiFi",                 /* UI_STR_WIFI */
    "WiFi Setup",           /* UI_STR_WIFI_SETUP */
    "Format",               /* UI_STR_FORMAT */
    "Factory Reset",        /* UI_STR_FACTORY_RESET */
    "Language",             /* UI_STR_LANGUAGE */
    "On",                   /* UI_STR_ON */
    "Off",                  /* UI_STR_OFF */
    "Off",                  /* UI_STR_DISABLED */
    "Flip V",               /* UI_STR_FLIP_V */
    "Mirror",               /* UI_STR_FLIP_H */
    "Rotate 180",           /* UI_STR_FLIP_180 */
    "Auto",                 /* UI_STR_IR_AUTO */
    "Always On",            /* UI_STR_IR_ALWAYS */
    "Formatting...",        /* UI_STR_FORMATTING */
    "Formatted",            /* UI_STR_FORMAT_DONE */
    "Factory reset done",   /* UI_STR_FACTORY_DONE */
    "Format storage?\nAll files will be deleted",  /* UI_STR_CONFIRM_FORMAT */
    "Factory reset?",       /* UI_STR_CONFIRM_FACTORY */
    "Scan QR in APP",       /* UI_STR_SCAN_QR */
    "Deleted",              /* UI_STR_DELETED */
    "Playing...",           /* UI_STR_PLAYING */
    "No Files",             /* UI_STR_NO_FILES */
    "Record failed",        /* UI_STR_REC_FAIL */
    "Recording",            /* UI_STR_REC_START */
    "Video saved",          /* UI_STR_REC_SAVED */
    "Photo failed",         /* UI_STR_PHOTO_FAIL */
    "Photo saved",          /* UI_STR_PHOTO_SAVED */
    "Timelapse failed",     /* UI_STR_TL_FAIL */
    "Timelapse on",         /* UI_STR_TL_RUNNING */
    "Timelapse saved",      /* UI_STR_TL_SAVED */
    "Storage full",         /* UI_STR_SD_FULL */
    "Confirm?",             /* UI_STR_CONFIRM_TITLE */
    "Cancel",               /* UI_STR_CANCEL */
    "OK",                   /* UI_STR_CONFIRM_OK */
    "Processing...",        /* UI_STR_PROCESSING */
    "TL",                   /* UI_STR_ACT_TIMELAPSE */
    "Mode",                 /* UI_STR_HINT_SWITCH */
    "Back",                 /* UI_STR_BACK */
};

/* ------------------- 简体中文 ------------------- */
static const char *ui_str_zh_cn[] = {
    "录像",
    "拍照",
    "延时摄影",
    "回放",
    "设置",
    "模式菜单",
    "分辨率",
    "循环录像",
    "WDR宽动态",
    "画面翻转",
    "红外夜视",
    "录音",
    "时间水印",
    "WiFi",
    "WiFi配网",
    "格式化",
    "恢复出厂",
    "语言",
    "开",
    "关",
    "关闭",
    "上下翻",
    "左右镜像",
    "旋转180",
    "自动",
    "常开",
    "格式化中...",
    "格式化完成",
    "已恢复出厂设置",
    "确定格式化存储?\n所有文件将被删除",
    "确定恢复出厂设置?",
    "请用APP扫码配网",
    "已删除",
    "播放中...",
    "暂无文件",
    "录像启动失败",
    "开始录像",
    "录像已保存",
    "拍照失败",
    "已保存照片",
    "延时启动失败",
    "延时摄影中",
    "延时已保存",
    "存储已满",
    "确认操作?",
    "取消",
    "确认",
    "处理中...",
    "延时",
    "切模式",
    "返回",
};

/* ------------------- 繁體中文 ------------------- */
static const char *ui_str_zh_tw[] = {
    "錄影",
    "拍照",
    "縮時攝影",
    "回放",
    "設定",
    "模式選單",
    "解析度",
    "循環錄影",
    "WDR寬動態",
    "畫面翻轉",
    "紅外夜視",
    "錄音",
    "時間浮水印",
    "WiFi",
    "WiFi配網",
    "格式化",
    "回復原廠",
    "語言",
    "開",
    "關",
    "關閉",
    "上下翻",
    "左右鏡像",
    "旋轉180",
    "自動",
    "常開",
    "格式化中...",
    "格式化完成",
    "已回復原廠設定",
    "確定格式化儲存?\n所有檔案將被刪除",
    "確定回復原廠設定?",
    "請用APP掃碼配網",
    "已刪除",
    "播放中...",
    "暫無檔案",
    "錄影啟動失敗",
    "開始錄影",
    "錄影已保存",
    "拍照失敗",
    "已保存照片",
    "縮時啟動失敗",
    "縮時攝影中",
    "縮時已保存",
    "儲存空間已滿",
    "確認操作?",
    "取消",
    "確認",
    "處理中...",
    "縮時",
    "切模式",
    "返回",
};

/* ------------------- 日本語 ------------------- */
static const char *ui_str_ja[] = {
    "録画",
    "撮影",
    "タイムラプス",
    "再生",
    "設定",
    "モードメニュー",
    "解像度",
    "ループ録画",
    "WDR",
    "画面反転",
    "赤外線ナイト",
    "マイク",
    "日時表示",
    "WiFi",
    "WiFi設定",
    "フォーマット",
    "出荷時設定",
    "言語",
    "オン",
    "オフ",
    "オフ",
    "上下反転",
    "左右反転",
    "180度回転",
    "自動",
    "常時オン",
    "フォーマット中...",
    "フォーマット完了",
    "初期化しました",
    "ストレージを初期化?\n全データが削除されます",
    "出荷時設定に戻す?",
    "APPでQRをスキャン",
    "削除しました",
    "再生中...",
    "ファイルなし",
    "録画開始失敗",
    "録画開始",
    "録画を保存",
    "撮影失敗",
    "写真を保存",
    "タイムラプス失敗",
    "タイムラプス中",
    "タイムラプス保存",
    "ストレージ満杯",
    "確認しますか?",
    "キャンセル",
    "OK",
    "処理中...",
    "タイムラプス",
    "切替",
    "戻る",
};

/* ------------------- Русский ------------------- */
static const char *ui_str_ru[] = {
    "Запись",
    "Фото",
    "Таймлапс",
    "Галерея",
    "Настройки",
    "Режимы",
    "Разрешение",
    "Цикл. запись",
    "WDR",
    "Поворот",
    "ИК-подсветка",
    "Микрофон",
    "Дата/время",
    "WiFi",
    "Wi-Fi настройка",
    "Форматирование",
    "Сброс настроек",
    "Язык",
    "Вкл",
    "Выкл",
    "Выкл",
    "Верт.",
    "Зеркало",
    "180 град.",
    "Авто",
    "Постоянно",
    "Форматирование...",
    "Готово",
    "Сброс выполнен",
    "Форматировать карту?\nФайлы будут удалены",
    "Сбросить настройки?",
    "Отсканируйте QR в APP",
    "Удалено",
    "Воспроизведение...",
    "Нет файлов",
    "Ошибка записи",
    "Запись начата",
    "Видео сохранено",
    "Ошибка фото",
    "Фото сохранено",
    "Ошибка таймлапса",
    "Таймлапс",
    "Таймлапс сохранён",
    "Память заполнена",
    "Подтвердить?",
    "Отмена",
    "OK",
    "Обработка...",
    "Таймлапс",
    "Режим",
    "Назад",
};

/* ------------------- ไทย ------------------- */
static const char *ui_str_th[] = {
    "บันทึกวิดีโอ",
    "ถ่ายภาพ",
    "ไทม์แลปส์",
    "ดูไฟล์",
    "ตั้งค่า",
    "เมนูโหมด",
    "ความละเอียด",
    "บันทึกวนซ้ำ",
    "WDR",
    "กลับภาพ",
    "กลางคืน",
    "ไมค์",
    "ลายน้ำเวลา",
    "WiFi",
    "ตั้งค่า WiFi",
    "ฟอร์แมต",
    "รีเซ็ตระบบ",
    "ภาษา",
    "เปิด",
    "ปิด",
    "ปิด",
    "พลิกขึ้นลง",
    "กลับข้าง",
    "หมุน 180",
    "อัตโนมัติ",
    "เปิดตลอด",
    "กำลังฟอร์แมต...",
    "ฟอร์แมตเสร็จ",
    "รีเซ็ตแล้ว",
    "ฟอร์แมตการ์ด?\nไฟล์ทั้งหมดจะถูกลบ",
    "รีเซ็ตระบบ?",
    "สแกน QR ในแอป",
    "ลบแล้ว",
    "กำลังเล่น...",
    "ไม่มีไฟล์",
    "บันทึกไม่สำเร็จ",
    "เริ่มบันทึก",
    "บันทึกแล้ว",
    "ถ่ายภาพไม่สำเร็จ",
    "บันทึกภาพแล้ว",
    "ไทม์แลปส์ไม่สำเร็จ",
    "กำลังไทม์แลปส์",
    "บันทึกไทม์แลปส์แล้ว",
    "พื้นที่เต็ม",
    "ยืนยัน?",
    "ยกเลิก",
    "ตกลง",
    "กำลังดำเนินการ...",
    "ไทม์แลปส์",
    "เปลี่ยนโหมด",
    "ย้อนกลับ",
};

/* ------------------- Tiếng Việt ------------------- */
static const char *ui_str_vi[] = {
    "Quay video",
    "Chụp ảnh",
    "Timelapse",
    "Xem lại",
    "Cài đặt",
    "Chế độ",
    "Độ phân giải",
    "Quay vòng",
    "WDR",
    "Lật ảnh",
    "Hồng ngoại",
    "Mic",
    "Mốc thời gian",
    "WiFi",
    "Cấu hình WiFi",
    "Định dạng",
    "Khôi phục gốc",
    "Ngôn ngữ",
    "Bật",
    "Tắt",
    "Tắt",
    "Lật dọc",
    "Lật ngang",
    "Xoay 180",
    "Tự động",
    "Luôn bật",
    "Đang định dạng...",
    "Định dạng xong",
    "Đã khôi phục gốc",
    "Định dạng thẻ?\nMọi tệp sẽ bị xóa",
    "Khôi phục gốc?",
    "Quét QR bằng APP",
    "Đã xóa",
    "Đang phát...",
    "Không có tệp",
    "Quay lỗi",
    "Bắt đầu quay",
    "Đã lưu video",
    "Chụp lỗi",
    "Đã lưu ảnh",
    "Timelapse lỗi",
    "Đang timelapse",
    "Đã lưu timelapse",
    "Bộ nhớ đầy",
    "Xác nhận?",
    "Hủy",
    "OK",
    "Đang xử lý...",
    "TL",
    "Chế độ",
    "Quay lại",
};

/* ------------------- Deutsch ------------------- */
static const char *ui_str_de[] = {
    "Video",
    "Foto",
    "Zeitraffer",
    "Wiedergabe",
    "Einstellungen",
    "Zurück",
    "Auflösung",
    "Loop-Aufnahme",
    "WDR",
    "Bildspiegelung",
    "IR-Nachtsicht",
    "Mikrofon",
    "Zeitstempel",
    "WiFi",
    "WiFi-Setup",
    "Formatieren",
    "Werksreset",
    "Sprache",
    "Ein",
    "Aus",
    "Aus",
    "Vertikal",
    "Spiegeln",
    "180° drehen",
    "Auto",
    "Dauerhaft",
    "Formatiere...",
    "Fertig",
    "Reset erfolgt",
    "SD formatieren?\nDateien werden gelöscht",
    "Werksreset durchführen?",
    "QR in der APP scannen",
    "Gelöscht",
    "Wiedergabe...",
    "Keine Dateien",
    "Aufnahme fehlgeschlagen",
    "Aufnahme gestartet",
    "Video gespeichert",
    "Foto fehlgeschlagen",
    "Foto gespeichert",
    "Zeitraffer fehlgeschlagen",
    "Zeitraffer läuft",
    "Zeitraffer gespeichert",
    "Speicher voll",
    "Bestätigen?",
    "Abbrechen",
    "OK",
    "Bitte warten...",
    "Zeitraffer",
    "Modus",
    "Menü",
};

/* ------------------- Français ------------------- */
static const char *ui_str_fr[] = {
    "Vidéo",
    "Photo",
    "Timelapse",
    "Lecture",
    "Réglages",
    "Modes",
    "Résolution",
    "Vidéo en boucle",
    "WDR",
    "Retournement",
    "Vision nocturne",
    "Micro",
    "Horodatage",
    "WiFi",
    "Config. WiFi",
    "Formatage",
    "Réinitialisation",
    "Langue",
    "Activé",
    "Désactivé",
    "Désactivé",
    "Vertical",
    "Miroir",
    "Rotation 180",
    "Auto",
    "Permanent",
    "Formatage...",
    "Formatage OK",
    "Réinitialisé",
    "Formater la carte?\nFichiers effacés",
    "Réinitialiser ?",
    "Scannez le QR dans l'APP",
    "Supprimé",
    "Lecture...",
    "Aucun fichier",
    "Échec vidéo",
    "Enregistrement",
    "Vidéo enregistrée",
    "Échec photo",
    "Photo enregistrée",
    "Échec timelapse",
    "Timelapse en cours",
    "Timelapse enregistré",
    "Mémoire pleine",
    "Confirmer ?",
    "Annuler",
    "OK",
    "Traitement...",
    "Timelapse",
    "Mode",
    "Retour",
};

/* ------------------- Italiano ------------------- */
static const char *ui_str_it[] = {
    "Video",
    "Foto",
    "Timelapse",
    "Galleria",
    "Impostazioni",
    "Modalità",
    "Risoluzione",
    "Video in loop",
    "WDR",
    "Capovolgi",
    "Visione notturna",
    "Microfono",
    "Data/ora",
    "WiFi",
    "Config. WiFi",
    "Formatta",
    "Ripristino",
    "Lingua",
    "On",
    "Off",
    "Off",
    "Verticale",
    "Specchia",
    "Ruota 180",
    "Auto",
    "Sempre",
    "Formattazione...",
    "Completato",
    "Ripristinato",
    "Formattare scheda?\nFile cancellati",
    "Ripristinare?",
    "Scansiona QR nell'APP",
    "Eliminato",
    "Riproduzione...",
    "Nessun file",
    "Errore video",
    "Registrazione",
    "Video salvato",
    "Errore foto",
    "Foto salvata",
    "Errore timelapse",
    "Timelapse attivo",
    "Timelapse salvato",
    "Memoria piena",
    "Confermi?",
    "Annulla",
    "OK",
    "In corso...",
    "Timelapse",
    "Modalità",
    "Indietro",
};

/* ------------------- Português ------------------- */
static const char *ui_str_pt[] = {
    "Vídeo",
    "Foto",
    "Timelapse",
    "Reprodução",
    "Ajustes",
    "Modos",
    "Resolução",
    "Gravação cíclica",
    "WDR",
    "Virar imagem",
    "Visão noturna",
    "Microfone",
    "Marca d'água",
    "WiFi",
    "Config. WiFi",
    "Formatar",
    "Restaurar padrão",
    "Idioma",
    "Ligado",
    "Desligado",
    "Desligado",
    "Vertical",
    "Espelhar",
    "Girar 180",
    "Auto",
    "Sempre",
    "Formatando...",
    "Concluído",
    "Padrão restaurado",
    "Formatar cartão?\nArquivos serão apagados",
    "Restaurar padrões?",
    "Escaneie o QR no APP",
    "Excluído",
    "Reproduzindo...",
    "Sem arquivos",
    "Falha na gravação",
    "Gravando",
    "Vídeo salvo",
    "Falha na foto",
    "Foto salva",
    "Falha no timelapse",
    "Timelapse ativo",
    "Timelapse salvo",
    "Memória cheia",
    "Confirmar?",
    "Cancelar",
    "OK",
    "Processando...",
    "Timelapse",
    "Modo",
    "Voltar",
};

/* 各语言表指针（下标 = ui_lang_id_t） */
static const char *const *s_tables[UI_LANG_MAX] = {
    [UI_LANG_EN]    = ui_str_en,
    [UI_LANG_ZH_CN] = ui_str_zh_cn,
    [UI_LANG_ZH_TW] = ui_str_zh_tw,
    [UI_LANG_JA]    = ui_str_ja,
    [UI_LANG_RU]    = ui_str_ru,
    [UI_LANG_TH]    = ui_str_th,
    [UI_LANG_VI]    = ui_str_vi,
    [UI_LANG_DE]    = ui_str_de,
    [UI_LANG_FR]    = ui_str_fr,
    [UI_LANG_IT]    = ui_str_it,
    [UI_LANG_PT]    = ui_str_pt,
};

/* 各语言自称（设置页语言选项展示，永远用各自母语写法，无需翻译） */
static const char *s_lang_names[UI_LANG_MAX] = {
    "English", "简体中文", "繁體中文", "日本語", "Русский",
    "ไทย", "Tiếng Việt", "Deutsch", "Français", "Italiano", "Português",
};

static ui_lang_id_t s_lang = UI_LANG_ZH_CN;

const char *ui_lang_tr(ui_str_id_t id)
{
    const char *s = NULL;

    if (id >= UI_STR_MAX) {
        return "";
    }
    if (s_tables[s_lang]) {
        s = s_tables[s_lang][id];
    }
    if (!s) {
        s = ui_str_en[id];      /* 回退英语 */
    }
    return s ? s : "";
}

const char *ui_lang_name(ui_lang_id_t lang)
{
    if (lang >= UI_LANG_MAX) {
        return "";
    }
    return s_lang_names[lang];
}

void ui_lang_set(ui_lang_id_t lang)
{
    if (lang < UI_LANG_MAX) {
        s_lang = lang;
    }
}

ui_lang_id_t ui_lang_get(void)
{
    return s_lang;
}
