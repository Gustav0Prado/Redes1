import random

deck = []

for i in range (0, 13):
    for j in range (0, i):
        deck.append(i)

#appending jesters
deck.append(13)
deck.append(13)
random.shuffle(deck)

personalDeck = []

print(deck)
print(personalDeck)
personalDeck = deck.pop()
print("Dando uma carta")
print(deck)
print(personalDeck)