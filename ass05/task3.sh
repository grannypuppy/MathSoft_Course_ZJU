# 统计 "war" 和 "peace" 出现的次数（不区分大小写，只匹配完整词）
war_count=$(grep -oi '\bwar\b' war_and_peace.txt | wc -w)
peace_count=$(grep -oi '\bpeace\b' war_and_peace.txt | wc -w)

echo "War appears $war_count times"
echo "Peace appears $peace_count times"
