# 能量机关：从 F4 修补到 H7 图形靶面

围绕五靶交互与灯光反馈，将旧 F4 方案逐步整理为带图形灯库、命中反馈与游戏流程的 H7 实现，并保留交付快照。

**当前分支**：`archive/f4-led-library`。F4 整合 LED 库。

**留存状态**：保留交付快照。

[查看项目时间线](docs/TIMELINE.md)

## 来源

继承 25 赛季 F4 能量机关方案；包含多板固件和早期 H7 尝试。个人与团队分工待补充。

个人职责待补充。保留原工程中的来源说明和许可证；本次归档没有进行构建、烧录或实机验收。

## 代码与历史分支

| 分支 | 内容 |
| --- | --- |
| `main` | 项目代表工作树，包含本次保存的现存源码。 |
| [`archive/h7-development`](https://github.com/RynStryn/energy-rune/tree/archive/h7-development) | H7 图形靶面开发工作树 |
| [`archive/f4-baseline`](https://github.com/RynStryn/energy-rune/tree/archive/f4-baseline) | F4 较早源码基线 |
| [`archive/f4-repair`](https://github.com/RynStryn/energy-rune/tree/archive/f4-repair) | F4 修补与多板方案 |
| [`archive/f4-led-library`](https://github.com/RynStryn/energy-rune/tree/archive/f4-led-library) | F4 整合 LED 库 |
| [`archive/h7-can-motor`](https://github.com/RynStryn/energy-rune/tree/archive/h7-can-motor) | 早期 H7 CAN／电机实现 |
| [`components/hexrec`](https://github.com/RynStryn/energy-rune/tree/components/hexrec) | F103 靶面接收板 |

## 时间口径

历史研发／修改日期写在时间线中。本次于 2026-11-04 整理并提交现存源码；新快照提交使用实际提交日期。

有原 Git 历史的工作树以原 HEAD 为父提交保留历史；无 Git 历史的树以独立根提交保存，不补造历史提交。工作区中的源码变动一并保存在本次快照。

本次快照排除了构建输出、IDE 会话文件和通用整理资料。历史提交保持原样，其中可能保留以前已跟踪的工具状态或构建文件。
