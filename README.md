# UE ProgramBrowser

使用虚幻引擎（Unreal Engine）提供的资源创建独立应用程序（Standalone Program）而非游戏（Not Game）

# 运行环境

- Unreal Engine 5.2.1 （源码版 Source Build）
- 插件形式（Plugin）

# 使用方法
- 放入Plugins目录下（Project或Engine）
- 重新构建Project或Engine以编译插件

# 功能

1. 使用模板代码创建独立应用程序 （类似插件管理器 Plugins Browser）
2. 一键式构建（Build），一键式打包（Package）
3. 方便可视化的Program管理
4. 打包后的Program可以直接运行，无需额外操作
4. ...

# 说明

- 可浏览的Program**需要**放在 `\Engine\Source\Program\Programs_Collection`下
- 打包后的Program放在项目路径下的`\Saved\Programs`下
- **只适用**于源码版引擎（安装版还在研究中）
- 功能还需完善...

# 参考资料

- https://zhuanlan.zhihu.com/p/391228179
- https://zhuanlan.zhihu.com/p/145633340

