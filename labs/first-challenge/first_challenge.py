nested_array = []

def getLength(elems):
  total_length = 0
  for elem in elems:
    if type(elem) is list:
      total_length += getLength(elem)
    else:
      total_length += 1
  return total_length

print(getLength(nested_array))