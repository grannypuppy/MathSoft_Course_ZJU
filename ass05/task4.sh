# 统计 BOOK 和 CHAPTER 的数量
book_count=$(grep -c "BOOK [A-Z]\+" war_and_peace.txt)
chapter_count=$(grep -c "CHAPTER [A-Z]\+" war_and_peace.txt)

echo "Total Books: $book_count"
echo "Total Chapters: $chapter_count"
