KRunner on Voy (aka Blade)
==========================

Ideas
-----

 - Matches provide a possibly longer formatted text to display to the user
   (dictionary result, parts of the text when searching file contents)
 - Action 'I don't want to see results like these' to disable specific
   runners, or maybe to set a keyword for a runner
 - Remote querying
 - If services runner returns the same application as shell runner,
   do not show two entries

Design
------

### Agents:


                  Local                 |        Remote
                                        |
             User Interface             |     User Interface
                    |                   |            |
                Collector               |        Collector
               /    |  \ \              |       /    |    \
         Runner  Runner | \             |  Runner  Runner Runner
                       /   \            +
                      /     \
            SharedRunner  SharedRunner     SharedRunner  SharedRunner

        Should Runners spin separate children for different Collectors?


### Messages


#### PingMessage, PongMessage

The `Collector` agent does not know which runners are available
until it sends the ping to all of them. All the runners should
reply with the pong message.



#### QueryMessage

When the user enters a search query, `Collector` sends the
query to all runners.

The query message needs to contain:

  - `(sender)` - `Collector` instance that sent the message
  - `id` - This can be an autoincremented value
  - `query` - the text that is typed in



#### ResultMessage

Each runner needs to save the last query id it received for
**each** of the different senders (multiple `Collector`s on different systems),
and consider that any response to an earlier query will be ignored.

  - `queryId` - which query are we responding to
  - `title` - The title for the match - this will be shown to the user
  - `description` - Description of the match - this might be shown to the user,
        depends on the UI
  - `icon` - Icon of the match
  - `matchedText` - The text in which the result appeared. This will not be
        shown to the user, it will be used to keep previous results in the list
        that match the updated query. (user types 'kru', gets krunner, types
        'nn', krunner stays in the list)
  - `data` - the data associated with this match; usually runner-specific
  - `id`
  - `category`
  - `mimeType`
  - `relevance`
  - `type` - ExactMatch, HelperMatch, Informational, Possible, Completion
  - `urls`



