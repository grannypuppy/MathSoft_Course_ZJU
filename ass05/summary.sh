# 检查是否提供了参数
if [ $# -eq 0 ]; then
    echo "Usage: $0 <keyword>"
    exit 1
fi

keyword=$1

# 统计关键词出现的次数（不区分大小写）
count=$(grep -oi "\b$keyword\b" war_and_peace.txt | wc -w)
echo "The keyword '$keyword' appears $count times."

# 输出包含该关键词的前5行内容
echo "First 5 lines containing the keyword:"
grep -i "\b$keyword\b" war_and_peace.txt | head -5 #不区分大小写
