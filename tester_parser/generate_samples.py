from random import randint

samples = 100

for i in range(samples):
	with open("./samples/" + str(i) + ".txt", 'w') as file:
		word = 'a' * randint(0, 25)
		word += 'b' * randint(0, 25)
		file.write(word)
		