def min_difference_partition(stones):
    total_sum = sum(stones)
    target = total_sum // 2
    
    dp = [False] * (target + 1)
    dp[0] = True
    
    for weight in stones:
        for j in range(target, weight - 1, -1):
            if dp[j - weight]:
                dp[j] = True
                
    for j in range(target, -1, -1):
        if dp[j]:
            first_heap_sum = j
            break
            
    second_heap_sum = total_sum - first_heap_sum
    return abs(first_heap_sum - second_heap_sum)

# Чтение входных данных
n = int(input())
stones = list(map(int, input().split()))

# Вычисление минимальной разности
result = min_difference_partition(stones)
print(result)