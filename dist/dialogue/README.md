# How to create dialogue trees
Currently using [Twine 2](https://twinery.org/2/#/) with the [twison](https://github.com/lazerwalker/twison) extension to author dialogue trees and export them into a json format. This is then read by the code during runtime for displaying dialogue.

### Text Format
Each passage within Twine is split into three regions:
- Parameter Dictionary: dictionary filled with any relevant parameters for the current dialogue passage. {} is used if no parameters are necessary
- Dialogue Section: the actual text to display for the dialogue
- Choices Section: the various choices to be displayed for dialogue divergence

An example passage is provided below:
```
{"character": "Narration"}
Your mind flashes back to the night before. How you decided to spend just another hour making dinner. How you stayed up till 5am watching League. And now you're showing up to class without a prototype to share.
[[Next->Story 4]]
```
