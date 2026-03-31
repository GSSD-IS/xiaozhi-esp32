# Enterprise Wi-Fi V1

## 范围

当前版本的企业 Wi‑Fi 目标是最小可交付版本：

- 支持普通 Wi‑Fi（PSK）
- 支持企业 Wi‑Fi（PEAP）
- 企业 Wi‑Fi 输入字段固定为：`SSID + Username + Password`

当前版本不包含：

- 自定义 `Identity`
- `CA Certificate`
- Hotspot 网页配置企业 Wi‑Fi

## 内部模型

固件内部统一使用 `NetworkProfile`：

- `auth_type = psk | peap`
- `ssid`
- `username`
- `password`

普通 Wi‑Fi 使用 `psk`。

企业 Wi‑Fi 使用 `peap`，并默认把 `username` 同时作为外层 identity 使用。

## 当前支持的配置入口

- BluFi
- 音频配网
- 旧版普通 Wi‑Fi 配置兼容回读

## 当前不支持的配置入口

- Hotspot 网页表单输入企业 Wi‑Fi

这项功能属于后续扩展，不算 V1 必交。

## 验证目标

V1 完成前至少需要通过以下验证：

- 普通 Wi‑Fi 能正常连接
- 升级后旧版普通 Wi‑Fi 配置仍能正常连接
- BluFi 可下发 PEAP 账号密码并成功连接
- 音频配网可下发 PEAP 账号密码并成功连接
- 企业 Wi‑Fi 密码错误时能失败并返回可重配状态

## 进度状态

当前进度状态请以 [plans/enterprise-wifi-v1-status.md](/Users/dreakchang/mommy/xiaozhi-esp32/plans/enterprise-wifi-v1-status.md) 为准。
