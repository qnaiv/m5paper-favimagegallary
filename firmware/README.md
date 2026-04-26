# firmware

M5Paper 向け PlatformIO プロジェクト。GitHub Pages から manifest.json を取得し、最新画像を E-ink に表示する。

## 必要環境

- [PlatformIO](https://platformio.org/) (VSCode 拡張 または CLI)
- M5Paper

## ビルド & 書き込み

```bash
# ビルド
pio run

# M5Paper に書き込み
pio run --target upload
```

## SD カード設定

`data/config.example.json` を参考に `config.json` を作成し、SD カードのルートに配置する。

```json
{
  "wifi_ssid": "YOUR_WIFI_SSID",
  "wifi_password": "YOUR_WIFI_PASSWORD",
  "manifest_url": "https://<username>.github.io/m5paper-favimagegallary/manifest.json",
  "update_interval_sec": 3600
}
```
