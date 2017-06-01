# Analysing Athlete Data using the Strava V3 API

## Glossary

### Strava Segment

Strava segments are member-created and edited portions of road or trail
where athletes can compete for time. Having completed a run or cycle, a
user can create a segment by positioning a start and end marker on a visual
representation of an activity they have completed. A leaderboard is created, and
associated with each new segment. A new segment may have a rather full
leaderboard as efforts made before the segment are also valid.

Climbing segments (for cyclists) are automatically categorized HC, 1, 2, 3 or
4, as seen in the Tour de France. (4 is a tough climb, but the easiest of the
5 categories.) Here are two Strava segments:

 [An uncategorised segment]( https://www.strava.com/segments/8498598)

 [A category 3 climb segment](https://www.strava.com/segments/8194858)

Important data from a segment includes its length, and its average gradient.
A 5% incline for 1 mile is quite different to the same over 5 miles. Steep
gradients, say over 20%, are very rare, and tend to be quite short. Although
challenging, many cyclists seek out such obstacles, and value them.
Scotland has plenty of hills and mountains, and so the scarcity of extremely
steep climbs is due to road planners; who seek gradual climbs for the
benefit of the majority of conventional road users. Note that there are also
segments created by mountain bike users. We are focused on road cycling,
and these results should ideally be filtered out. This may not
be easy, however, as many Strava users do not include their bike details.
Corroborating a segment against known roads could be one route to this.

You can also learn a lot from the number of names on the leaderboard. The second
segment linked above, "The Crow Road", is one of the most popular climbs in
Scotland. 21,091 attempts have been made by 5,276 people; including famous
cyclists such as Katie Archibald (in second position).

### KOM & QOM

King or Queen of the Mountain (KOM/QOM) is the award given to the athlete in
the top position on the leaderboard.

### Strava Route

Unlike a Strava segment, a Strava route can be freely created using mouse-driven
route mapping tools provided within the Strava website. There is also no
leaderboard associated with a route; though a list of segments, through which
the route passes, is generated. Here is a Strava route:

 [A Strava route](https://www.strava.com/routes/5565831)

## Ideas

The following are a set of potential applications of the Strava API:

 * Rank the most popular segments of a geographic location. Perhaps filter
them by the climb category; or by distance.

 * Given a planned route, and a time of day (e.g. right now), provide a list
of segments which are aligned with the wind. Wind direction can be input, or
obtained through a weather API.

 * Use Strava to generate routes that outline an interesting silhouette; or
text message, such as someone's name.

 * Search for local weak segments, with an opportunity to claim a KOM. Perhaps
a low number of attempts on the leaderboard; a well-spaced time distribution;
low power; or even low speed.

 * Given a strava segment, report the best *approach*; this will likely be
that chosen by the top 10 on the leaderboard.

* Score each Strava user by their ten (say) best places on segment leaderboards;
scaled by the difficulty of each such segment (John Nixon).

 * Calculate the wind speed based on the live segment times of a large number of
commuters, taking into account the orientation of the segments.

 * Calculate new, local categorised hill ascents: group existing ascending
segments and, if necessary, temporarily add artifical GPS data.

 * Calculate the route between points A & B which has the best ratio of ascent
to distance. This would require specifying a maximum and minumum distance.

 * Search for or create strava routes, excluding mountain bike segments. Could check whether
a conventional road exists; or how many cyclists have taken the route (often
low for mountain bikes); or possibly whether the type of bike used has been
listed.

# Questions Remaining

What credentials from Strava does a user cloning our repo require?

Can the data of a journey be artifically generated and used to submit a journey
to Strava?

Can the Strava API tell me my current GPS coordinates?

Strava *segments* may only be using sequences of time-stamped GPS coordinates.
That is, they have no knowledge of their proximity to place names; and may even
be off-road. Nevertheless, Strava *routes* use maps and roads, and will offer
default route options between points on the map. Does the Strava API allow
routes to be generated which pass through certain named locations? Can routes
be generated which include particular segments?

# Proposed Task Sequence

* A cross-platform C++ library wrapping the functionality of the Strava API 

* In realising ideas from the section above, each should become a separate
program; ideally a single cpp file. A GUI would be nice, but takes time;
for now we are looking to demonstrate the concept.

* The repository will be private for now - a license will be decided later


