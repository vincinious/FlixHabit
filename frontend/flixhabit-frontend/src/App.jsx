import React, { useEffect, useState } from "react";
import { Card, CardHeader, CardContent } from "./components/card.jsx";
import { Tabs, TabsList, TabsTrigger, TabsContent } from "./components/tabs.jsx";
import { ScrollArea } from "./components/scroll-area.jsx";
import {
  BarChart,
  Bar,
  XAxis,
  YAxis,
  Tooltip,
  CartesianGrid,
  ResponsiveContainer,
} from "recharts";
import { motion } from "framer-motion";
import { Users, Video } from "lucide-react";

/* Netflix‑style dashboard -------------------------------------------------- */

export default function App() {
  const [ageBuckets, setAgeBuckets] = useState([]);
  const [watchTime, setWatchTime] = useState({});
  const [similarPairs, setSimilarPairs] = useState([]);
  const [basic, setBasic] = useState([]);
  const [standard, setStandard] = useState([]);
  const [premium, setPremium] = useState([]);
  const [activeUsers, setActiveUsers] = useState([]);
  const [loading, setLoading] = useState(true);
  const [error, setError] = useState(null);
  const [tab, setTab] = useState("overview");

  /* ------------ fetch all JSON files ------------ */
  useEffect(() => {
    const fetchJson = async (file) => {
      const res = await fetch(`/data/${file}`);
      if (!res.ok)
        throw new Error(`${file} not found — did you copy it to public/data?`);
      return res.json();
    };

    Promise.all([
      fetchJson("genreForAgeGroup.json"),
      fetchJson("avgWatchTimeByCountry.json"),
      fetchJson("similar_users.json"),
      fetchJson("basic_users.json"),
      fetchJson("Standard_users.json"),
      fetchJson("Premium_users.json").catch(() => []),
      fetchJson("topActive_users.json"),
    ])
      .then(
        ([
          buckets,
          watch,
          pairs,
          basicU,
          standardU,
          premiumU,
          activeU,
        ]) => {
          setAgeBuckets(buckets);
          setWatchTime(watch);
          setSimilarPairs(pairs);
          setBasic(basicU);
          setStandard(standardU);
          setPremium(premiumU);
          setActiveUsers(activeU);
        },
      )
      .catch((e) => setError(e.message))
      .finally(() => setLoading(false));
  }, []);

  /* -------------- derived data -------------- */
  const watchTimeData = Object.entries(watchTime).map(([country, hours]) => ({
    country,
    hours,
  }));

  const subCounts = [
    { tier: "Basic", count: basic.length },
    { tier: "Standard", count: standard.length },
    { tier: "Premium", count: premium.length },
  ];

  /* -------------- early returns -------------- */
  if (loading)
    return (
      <div className="flex h-screen items-center justify-center bg-neutral-900 text-neutral-100">
        Loading…
      </div>
    );
  if (error) return <div className="p-4 text-red-500">{error}</div>;

  /* -------------- UI -------------- */
  return (
    <div className="dark min-h-screen bg-neutral-900 font-inter text-neutral-100 pb-4">
      <ScrollArea className="h-screen w-screen p-6">
        <motion.div
          initial={{ opacity: 0, y: -20 }}
          animate={{ opacity: 1, y: 0 }}
          transition={{ duration: 0.4 }}
          className="mx-auto flex max-w-6xl flex-col gap-6"
        >
          <header className="flex items-center gap-2 text-3xl font-bold text-red-600">
            <Video className="h-6 w-6" /> FlixHabit Dashboard
          </header>

          {/* ---------------- Tabs ---------------- */}
          <Tabs value={tab} onValueChange={setTab}>
            <TabsList className="mb-4">
              <TabsTrigger value="overview">Overview</TabsTrigger>
              <TabsTrigger value="subscriptions">Subscriptions</TabsTrigger>
              <TabsTrigger value="users">Users</TabsTrigger>
            </TabsList>

            {/* ---------- Overview ---------- */}
            <TabsContent value="overview">
              <div className="grid gap-6 md:grid-cols-2">
                {/* Genre by age */}
                <Card>
                  <CardHeader>Most common genre by age range</CardHeader>
                  <CardContent>
                    <table className="w-full text-sm">
                      <tbody>
                        {ageBuckets.map((b) => (
                          <tr key={b.ageRange} className="even:bg-neutral-800/40">
                            <td className="py-1 pl-2">{b.ageRange}</td>
                            <td className="py-1 pr-2 text-right text-red-400">
                              {b.genre}
                            </td>
                          </tr>
                        ))}
                      </tbody>
                    </table>
                  </CardContent>
                </Card>

                {/* Watch time chart */}
                <Card>
                  <CardHeader>Average watch time by country (hours)</CardHeader>
                  <CardContent className="h-64">
                    {watchTimeData.length ? (
                      <ResponsiveContainer width="100%" height="100%">
                        <BarChart data={watchTimeData}>
                          <CartesianGrid strokeDasharray="3 3" stroke="#444" />
                          <XAxis dataKey="country" stroke="#ccc" />
                          <YAxis stroke="#ccc" />
                          <Tooltip
                            contentStyle={{
                              background: "#1f1f1f",
                              border: "none",
                            }}
                          />
                          <Bar
                            dataKey="hours"
                            fill="#e50914"
                            radius={[4, 4, 0, 0]}
                          />
                        </BarChart>
                      </ResponsiveContainer>
                    ) : (
                      <div className="flex h-full items-center justify-center text-neutral-500">
                        No data
                      </div>
                    )}
                  </CardContent>
                </Card>
              </div>
            </TabsContent>

            {/* ---------- Subscriptions ---------- */}
            <TabsContent value="subscriptions">
              <div className="grid gap-6 md:grid-cols-3">
                {subCounts.map((s) => (
                  <Card
                    key={s.tier}
                    className="flex flex-col items-center justify-center gap-2 py-8 transition-all hover:shadow-lg hover:ring-red-600"
                  >
                    <Users className="h-8 w-8 text-red-600" />
                    <CardHeader className="text-xl font-semibold text-red-500">
                      {s.tier}
                    </CardHeader>
                    <CardContent className="text-4xl font-bold">
                      {s.count}
                    </CardContent>
                  </Card>
                ))}
              </div>
            </TabsContent>

            {/* ---------- Users ---------- */}
            <TabsContent value="users">
              <div className="grid gap-6 md:grid-cols-2">
                {/* Similar pairs */}
                <Card>
                  <CardHeader>Top similar user pairs</CardHeader>
                  <CardContent className="max-h-64 overflow-auto pr-2">
                    <ul className="space-y-1 text-sm">
                      {similarPairs.map((p) => (
                        <li
                          key={`${p.user1ID}-${p.user2ID}`}
                          className="flex justify-between"
                        >
                          <span>
                            User {p.user1ID} &amp; {p.user2ID}
                          </span>
                          <span className="text-red-400">
                            {p.similarity.toFixed(1)}
                          </span>
                        </li>
                      ))}
                    </ul>
                  </CardContent>
                </Card>

                {/* Most active */}
                <Card>
                  <CardHeader>Most active users</CardHeader>
                  <CardContent className="max-h-64 overflow-auto pr-2">
                    <ul className="space-y-1 text-sm">
                      {activeUsers.map((u) => (
                        <li
                          key={u.userID}
                          className="flex justify-between truncate"
                        >
                          <span>
                            {u.name}{" "}
                            <span className="text-neutral-500">
                              ({u.genre})
                            </span>
                          </span>
                          <span className="text-red-400">
                            {u.watchTime.toFixed(1)}h
                          </span>
                        </li>
                      ))}
                    </ul>
                  </CardContent>
                </Card>
              </div>
            </TabsContent>
          </Tabs>
        </motion.div>
      </ScrollArea>
    </div>
  );
}
