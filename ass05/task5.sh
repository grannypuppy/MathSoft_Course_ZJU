# 提取 BOOK FIVE 到 BOOK SIX 之间的内容
mkdir -p samples
sed -n '/BOOK FIVE/,/BOOK SIX/p' war_and_peace.txt > samples/book5.txt
