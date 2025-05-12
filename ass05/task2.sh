# 找出出现次数最多的前10个单词
cat war_and_peace.txt | 
  tr '[:upper:]' '[:lower:]' |  # 转换为小写
  grep -o '\b[a-z]\+\b' |       # 只提取纯小写字母单词
  sort |                        # 排序
  uniq -c |                     # 计数
  sort -nr |                    # 按数字逆序排序
  head -10 |                    # 取前10个
  awk '{printf "%-10s %d\n", $2, $1}'  # 格式化输出
