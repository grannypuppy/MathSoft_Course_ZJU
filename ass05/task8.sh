# 找出整本书中最长的单词及其出现次数
longest_word=$(grep -o '\b[A-Za-z]\+\b' war_and_peace.txt | 
              awk '{ if (length($0) > max_length) { max_length = length($0); max_word = $0 } } 
                   END { print max_word }')

count=$(grep -oi "\b$longest_word\b" war_and_peace.txt | wc -w)

echo "Longest word: $longest_word (with ${#longest_word} characters)"
echo "It appears $count times in the text."
