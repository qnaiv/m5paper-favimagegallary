# M5Paper Firmware

M5Paper向けファームウェア。SDカードの `config.json` からWiFi/サーバURL設定を読み込み、起動時に接続情報・時刻・バッテリー残量を画面に表示します。

## 必要なもの

- [PlatformIO](https://platformio.org/) (VS Code拡張またはCLI)
- M5Paper本体
- microSDカード (FAT32フォーマット)
- USBケーブル (USB-C)

## SDカード準備

1. `firmware/data/config.example.json` を参考に `config.json` を作成します。

```json
{
  "wifi_ssid": "YOUR_WIFI_SSID",
  "wifi_password": "YOUR_WIFI_PASSWORD",
  "server_base_url": "https://qnaiv.github.io/m5paper-favimagegallary/",
  "update_interval_sec": 3600
}
```

2. microSDカードをFAT32でフォーマットし、`config.json` をルート直下に配置します。

```
/config.json   ← ここに置く
```

3. SDカードをM5Paperに挿入します。

> **注意**: `config.json` にはWiFiパスワードが含まれます。Gitにコミットしないよう `.gitignore` で除外済みです。

## ビルド・書き込み手順

### VS Code + PlatformIO拡張の場合

1. VS CodeでPlatformIO拡張をインストール
2. `firmware/` フォルダをVS Codeで開く
3. 左サイドバーのPlatformIOアイコン → **Build** でビルド
4. M5Paperをパソコンに接続し、**Upload** で書き込み

### CLIの場合

```bash
cd firmware

# ビルド
pio run

# 書き込み (ポートは自動検出)
pio run --target upload

# シリアルモニタ
pio device monitor --baud 115200
```

シリアルポートを明示する場合:

```bash
pio run --target upload --upload-port /dev/ttyUSB0
pio device monitor --port /dev/ttyUSB0 --baud 115200
```

## 動作確認手順

1. SDカードに `config.json` を置いてM5Paperに挿入
2. ファームウェアを書き込み
3. M5Paperを再起動（リセットボタンまたは電源OFF/ON）
4. 起動シーケンス:
   - 「Booting...」表示 → SDマウント → config読み込み → WiFi接続 → NTP同期
   - 成功時: SSID / IP / 時刻 / バッテリー残量を表示
   - 失敗時: `[ERROR]` メッセージを表示し、電源を保持（deep sleepしない）

5. シリアルモニタでデバッグログを確認:

```
[Boot] Start
[SD] Mounted
[Config] ssid=MyWiFi url=https://... interval=3600
[WiFi] Connecting to: MyWiFi
....
[WiFi] Connected, IP: 192.168.1.100
[NTP] Syncing... done: 2026-04-26 12:00:00
[Status] SSID=MyWiFi IP=192.168.1.100 Time=2026-04-26 12:00:00 Batt=85%
[Boot] Complete
```

## トラブルシューティング

| 症状 | 原因 | 対処 |
|------|------|------|
| `SD mount failed` | SDカード未挿入またはフォーマット不正 | FAT32でフォーマットして再挿入 |
| `config.json read failed` | ファイルなし / JSON構文エラー | ルートに `config.json` を正しく配置 |
| `WiFi connect failed` | SSID/パスワード誤り / 電波届かない | `config.json` の値を確認 |
| 時刻が `--:--:--` | NTP同期タイムアウト | WiFi接続後にNTPサーバへ疎通確認 |

## ボード設定について

`platformio.ini` では `board = m5stack-fire` を使用しています。M5Paper専用のPlatformIOボード定義が存在しないため、フラッシュサイズ(16MB)が同じ `m5stack-fire` で代替しています。M5Unified がランタイムでM5Paperハードウェアを自動検出するため、表示・電源管理は正常に動作します。
