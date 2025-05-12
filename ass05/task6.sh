# 找出包含 "Napoleon" 的行，并统计行数
grep "Napoleon" war_and_peace.txt > napoleon_lines.txt
line_count=$(wc -l < napoleon_lines.txt)

echo "Lines containing 'Napoleon': $line_count"
