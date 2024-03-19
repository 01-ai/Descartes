# Descartes Java 接口文档

1. 将 [libdescartes.so](/lib/libdescartes.so) 放入项目根目录或者系统库中
2. 在项目中引入 `descartes.jar`
3. 参考 [sample.java](/java/src/com/lingyiwanwu/descartes/example/Sample.java)，对接 Descartes，大致接口调用的流程如下：
    * 首先准备一份[配置文件](/java/src/com/lingyiwanwu/descartes/example/Sample.config)
    * `init`：指定配置文件路径初始化向量索引，如果 `vector.global.index_dir` 目录已经存在 dump 过的数据，则会自动加载
    * `addVector`：插入一条向量，维度与 `vector.global.dimension` 匹配
    * `refineIndex`：待所有向量插入完成后，调用该接口来优化索引结构
    * `dump`：构建数据 dump 到 `vector.global.index_dir` 目录
    * `close`：不再使用后，手动释放资源