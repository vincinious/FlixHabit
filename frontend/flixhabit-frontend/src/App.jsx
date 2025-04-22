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
import { motion, AnimatePresence } from "framer-motion";
import { Users, Video, Clock, Star, Activity, AlertTriangle, Menu, X } from "lucide-react";
import { cn } from "./lib/utils";
import GraphDisplay from './components/GraphDisplay';

/* Netflix‑style dashboard -------------------------------------------------- */

const CustomTooltip = ({ active, payload, label }) => {
  if (active && payload && payload.length) {
    return (
      <div className="rounded-lg border border-neutral-700 bg-neutral-800/90 p-3 text-sm shadow-lg backdrop-blur-sm">
        <p className="font-semibold text-neutral-200">{label}</p>
        <p className="text-red-400">{`Avg Watch Time: ${payload[0].value.toFixed(1)} hours`}</p>
      </div>
    );
  }
  return null;
};

const GrainEffect = () => (
  <div className="pointer-events-none fixed inset-0 z-40 overflow-hidden">
    <div className="absolute inset-0 animate-grain bg-[url('data:image/svg+xml;base64,PHN2ZyB4bWxucz0iaHR0cDovL3d3dy53My5vcmcvMjAwMC9zdmciIHdpZHRoPSIzMDAiIGhlaWdodD0iMzAwIj4NCiAgICA8ZmlsdGVyIGlkPSJub2lzZSIgeD0iMCIgeT0iMCI+DQogICAgICAgIDxmZVR1cmJ1bGVuY2UgdHlwZT0iZnJhY3RhbE5vaXNlIiBiYXNlRnJlcXVlbmN5PSIwLjciIHN0aXRjaFRpbGVzPSJzdGl0Y2giLz4NCiAgICA8L2ZpbHRlcj4NCiAgICA8cmVjdCB3aWR0aD0iMTAwJSIgaGVpZ2h0PSIxMDAlIiBmaWx0ZXI9InVybCgjbm9pc2UpIiBvcGFjaXR5PSIwLjA3Ii8+DQo8L3N2Zz4NCg==')] opacity-40"></div>
  </div>
);


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
  const [isMobile, setIsMobile] = useState(false);
  const [menuOpen, setMenuOpen] = useState(false);
  const [mobileMenuOpen, setMobileMenuOpen] = useState(false);
  const [data, setData] = useState([]);
  const [recommendation, setRecommendation] = useState([]);

  useEffect(() => {
    const handleResize = () => {
      const mobile = window.innerWidth < 768;
      setIsMobile(mobile);
      if (!mobile && menuOpen) {
        setMenuOpen(false);
      }
    };
    handleResize();
    window.addEventListener('resize', handleResize);
    return () => window.removeEventListener('resize', handleResize);
  }, [menuOpen]);

  useEffect(() => {
    const handleClickOutside = (e) => {
      if (menuOpen && !e.target.closest('.mobile-menu') && !e.target.closest('.menu-button')) {
        setMenuOpen(false);
      }
    };
    if (menuOpen) {
      document.addEventListener('mousedown', handleClickOutside);
    }
    return () => {
      document.removeEventListener('mousedown', handleClickOutside);
    };
  }, [menuOpen]);

  useEffect(() => {
    const fetchJson = async (file) => {
      const res = await fetch(`/data/${file}`);
      if (!res.ok)
        throw new Error(`${file} not found — did you copy it to public/data?`);
      return res.json();
    };

    const loadData = async () => {
      setLoading(true);
      setError(null);
      try {
        const [
          buckets,
          watch,
          pairs,
          basicU,
          standardU,
          premiumU,
          activeU,
        ] = await Promise.all([
          fetchJson("genreForAgeGroup.json"),
          fetchJson("avgWatchTimeByCountry.json"),
          fetchJson("similar_users.json"),
          fetchJson("basic_users.json"),
          fetchJson("Standard_users.json"),
          fetchJson("Premium_users.json").catch(() => []),
          fetchJson("topActive_users.json"),
        ]);

        await new Promise(resolve => setTimeout(resolve, 800));

        setAgeBuckets(buckets);
        setWatchTime(watch);
        setSimilarPairs(pairs);
        setBasic(basicU);
        setStandard(standardU);
        setPremium(premiumU);
        setActiveUsers(activeU);
      } catch (e) {
        console.error("Error fetching dashboard data:", e);
        setError(e.message || "Failed to load dashboard data.");
      } finally {
        setLoading(false);
      }
    };

    loadData();
  }, []);

  const watchTimeData = Object.entries(watchTime).map(([country, hours]) => ({
    country,
    hours,
  }));

  const subCounts = [
    { tier: "Basic", count: basic.length, icon: <Users className="h-6 w-6" />, color: "from-blue-500 to-blue-600", shadowColor: "shadow-blue-500/30" },
    { tier: "Standard", count: standard.length, icon: <Star className="h-6 w-6" />, color: "from-purple-500 to-purple-600", shadowColor: "shadow-purple-500/30" },
    { tier: "Premium", count: premium.length, icon: <Activity className="h-6 w-6" />, color: "from-red-500 to-red-600", shadowColor: "shadow-red-500/30" },
  ];

  const containerVariants = {
    hidden: { opacity: 0 },
    visible: { opacity: 1, transition: { staggerChildren: 0.1 } },
  };

  const itemVariants = {
    hidden: { y: 20, opacity: 0 },
    visible: { y: 0, opacity: 1, transition: { type: "spring", stiffness: 100 } },
  };

  const mobileMenuVariants = {
    closed: { opacity: 0, x: "100%", transition: { type: "spring", stiffness: 300, damping: 30 } },
    open: { opacity: 1, x: "0%", transition: { type: "spring", stiffness: 300, damping: 30, staggerChildren: 0.1, delayChildren: 0.2 } }
  };

  const menuItemVariants = {
    closed: { x: 20, opacity: 0 },
    open: { x: 0, opacity: 1 }
  };

  const toggleMobileMenu = () => {
    setMobileMenuOpen(!mobileMenuOpen);
  };

  const closeMobileMenu = () => {
    setMobileMenuOpen(false);
  };

  if (loading)
    return (
      <div className="flex h-screen items-center justify-center bg-neutral-950 text-neutral-300">
        <GrainEffect />
        <div className="flex flex-col items-center gap-4">
          <motion.div 
            animate={{ rotate: 360 }}
            transition={{ duration: 1, repeat: Infinity, ease: "linear" }}
            className="h-12 w-12 rounded-full border-4 border-red-600 border-t-transparent"
          />
          <p className="text-lg font-medium tracking-wider">Loading Dashboard...</p>
        </div>
      </div>
    );
  if (error) return (
    <div className="flex h-screen items-center justify-center bg-neutral-950 p-4">
      <GrainEffect />
      <motion.div 
        initial={{ opacity: 0, scale: 0.9 }}
        animate={{ opacity: 1, scale: 1 }}
        className="w-full max-w-md rounded-xl border border-red-900/50 bg-red-900/20 p-6 text-center shadow-2xl shadow-red-900/20 backdrop-blur-sm"
      >
        <AlertTriangle className="mx-auto mb-4 h-12 w-12 text-red-500" />
        <h2 className="mb-2 text-xl font-bold text-red-400">Error Loading Data</h2>
        <p className="text-neutral-300">{error || "An unexpected error occurred."}</p>
        <button 
          onClick={() => window.location.reload()}
          className="mt-6 rounded-md bg-red-600 px-4 py-2 text-sm font-semibold text-white transition-colors hover:bg-red-700"
        >
          Try Again
        </button>
      </motion.div>
    </div>
  );

  return (
    <div className="flex flex-col h-screen overflow-x-hidden">
      <nav className="flex items-center justify-between p-4 bg-black border-b border-gray-700">
        <div className="flex items-center">
          <img
            src="/netflix-logo.jpg"
            alt="FlixHabit Logo"
            width={40}
            height={40}
            className="mr-2"
          />
          <h1 className="text-2xl font-bold text-white">FlixHabit</h1>
        </div>
      </nav>


      <main className="flex-1 overflow-y-auto bg-black p-4">
        <div className="dark relative min-h-screen bg-gradient-to-b from-neutral-950 via-neutral-900 to-neutral-950 font-inter text-neutral-200 pb-8">
          <GrainEffect />
          <ScrollArea className="h-full w-full px-2 py-4 sm:px-6 lg:px-8">
            <motion.div
              initial={{ opacity: 0, y: -20 }}
              animate={{ opacity: 1, y: 0 }}
              transition={{ duration: 0.5, ease: "easeOut" }}
              className="mx-auto flex max-w-7xl flex-col gap-6 md:gap-8"
            >
              <header className="flex items-center justify-between gap-3 px-2 sm:px-0">
                <div className="flex items-center gap-3">
                  <div className="flex shrink-0 items-center gap-2 rounded-lg bg-red-600 px-3 py-2 shadow-lg shadow-red-600/30">
                    <Video className="h-5 w-5 sm:h-6 sm:w-6 text-white" />
                    <span className="bg-gradient-to-r from-white to-neutral-300 bg-clip-text text-transparent text-lg sm:text-2xl font-bold">FlixHabit</span>
                  </div>
                  <span className="ml-1 hidden text-neutral-400 sm:inline-block sm:text-2xl group relative font-bold">
                    Dashboard
                    <span className="absolute bottom-0 left-0 h-0.5 w-0 bg-red-600 transition-all duration-300 group-hover:w-full"></span>
                  </span>
                </div>
                <Tabs value={tab} onValueChange={setTab} className="hidden sm:block">
                  <TabsList className="border border-neutral-700/50 bg-neutral-800/50 backdrop-blur-sm">
                    <TabsTrigger value="overview">Overview</TabsTrigger>
                    <TabsTrigger value="subscriptions">Subscriptions</TabsTrigger>
                    <TabsTrigger value="users">Users</TabsTrigger>
                    <TabsTrigger value="graph">Graph</TabsTrigger>
                  </TabsList>
                </Tabs>
              </header>

              <Tabs value={tab} onValueChange={setTab} className="block sm:hidden px-2">
                <TabsList className="grid w-full grid-cols-4 border border-neutral-700/50 bg-neutral-800/50 backdrop-blur-sm">
                  <TabsTrigger value="overview">Overview</TabsTrigger>
                  <TabsTrigger value="subscriptions">Subs</TabsTrigger>
                  <TabsTrigger value="users">Users</TabsTrigger>
                  <TabsTrigger value="graph">Graph</TabsTrigger>
                </TabsList>
              </Tabs>

              <AnimatePresence mode="wait">
                <motion.div
                  key={tab}
                  initial={{ opacity: 0, y: 10 }}
                  animate={{ opacity: 1, y: 0 }}
                  exit={{ opacity: 0, y: -10 }}
                  transition={{ duration: 0.3 }}
                >
                  {tab === 'overview' && (
                    <motion.div
                      className="grid gap-4 sm:gap-6 md:grid-cols-2 lg:grid-cols-3"
                      variants={containerVariants}
                      initial="hidden"
                      animate="visible"
                    >
                      <motion.div variants={itemVariants} className={isMobile ? "order-2" : ""}>
                        <Card className="group overflow-hidden rounded-xl border border-neutral-700/50 bg-neutral-800/50 shadow-lg backdrop-blur-sm transition-all duration-300 hover:border-neutral-600/80 hover:shadow-xl hover:shadow-red-900/20">
                          <CardHeader className="border-b border-neutral-700/50 bg-gradient-to-b from-neutral-750/60 to-neutral-800/40 p-3 sm:px-6 sm:py-4">
                            <div className="flex items-center gap-2">
                              <Clock className="h-5 w-5 text-red-500 transition-transform duration-300 group-hover:scale-110" />
                              <h3 className="text-base sm:text-lg font-semibold text-neutral-100">Genre by Age</h3>
                            </div>
                          </CardHeader>
                          <CardContent className="p-0">
                            <table className="w-full text-sm">
                              <thead className="sr-only">
                                <tr><th>Age Range</th><th>Genre</th></tr>
                              </thead>
                              <tbody>
                                {ageBuckets.map((b, i) => (
                                  <tr
                                    key={b.ageRange}
                                    className={cn(
                                      "border-t border-neutral-700/30 transition-colors hover:bg-neutral-700/40",
                                      i === 0 && "border-t-0"
                                    )}
                                  >
                                    <td className="py-2 sm:py-3 pl-4 sm:pl-6 text-neutral-300">{b.ageRange}</td>
                                    <td className="py-2 sm:py-3 pr-4 sm:pr-6 text-right font-medium text-red-400">
                                      {b.genre}
                                    </td>
                                  </tr>
                                ))}
                              </tbody>
                            </table>
                          </CardContent>
                        </Card>
                      </motion.div>

                      <motion.div variants={itemVariants} className={`lg:col-span-2 ${isMobile ? "order-1" : ""}`}>
                        <Card className="group overflow-hidden rounded-xl border border-neutral-700/50 bg-neutral-800/50 shadow-lg backdrop-blur-sm transition-all duration-300 hover:border-neutral-600/80 hover:shadow-xl hover:shadow-red-900/20">
                          <CardHeader className="border-b border-neutral-700/50 bg-gradient-to-b from-neutral-750/60 to-neutral-800/40 p-3 sm:px-6 sm:py-4">
                            <div className="flex items-center gap-2">
                              <Activity className="h-5 w-5 text-red-500 transition-transform duration-300 group-hover:scale-110" />
                              <h3 className="text-base sm:text-lg font-semibold text-neutral-100">Avg. Watch Time (Hours)</h3>
                            </div>
                          </CardHeader>
                          <CardContent className="h-60 sm:h-72 p-4 pr-6 pb-6">
                            {watchTimeData.length ? (
                              <ResponsiveContainer width="100%" height="100%">
                                <BarChart data={watchTimeData} margin={{ top: 5, right: 0, left: isMobile ? -25 : -20, bottom: 5 }}>
                                  <defs>
                                    <linearGradient id="colorGradient" x1="0" y1="0" x2="0" y2="1">
                                      <stop offset="5%" stopColor="#e50914" stopOpacity={0.9}/>
                                      <stop offset="95%" stopColor="#b2070f" stopOpacity={0.7}/>
                                    </linearGradient>
                                  </defs>
                                  <CartesianGrid strokeDasharray="3 3" stroke="#ffffff1a" vertical={false} />
                                  <XAxis dataKey="country" stroke="#888" fontSize={isMobile ? 10 : 12} tickLine={false} axisLine={false} />
                                  <YAxis stroke="#888" fontSize={isMobile ? 10 : 12} tickLine={false} axisLine={false} />
                                  <Tooltip
                                    content={<CustomTooltip />}
                                    cursor={{ fill: "rgba(229, 9, 20, 0.15)" }}
                                  />
                                  <Bar
                                    dataKey="hours"
                                    fill="url(#colorGradient)"
                                    radius={[4, 4, 0, 0]}
                                  >
                                  </Bar>
                                </BarChart>
                              </ResponsiveContainer>
                            ) : (
                              <div className="flex h-full items-center justify-center text-neutral-500">
                                No watch time data available
                              </div>
                            )}
                          </CardContent>
                        </Card>
                      </motion.div>
                    </motion.div>
                  )}

                  {tab === 'subscriptions' && (
                    <motion.div
                      className="grid gap-4 sm:gap-6 md:grid-cols-3"
                      variants={containerVariants}
                      initial="hidden"
                      animate="visible"
                    >
                      {subCounts.map((s) => (
                        <motion.div
                          key={s.tier}
                          variants={itemVariants}
                          whileHover={{ y: -5, scale: 1.03 }}
                          transition={{ type: "spring", stiffness: 300, damping: 15 }}
                        >
                          <Card className={cn(
                            "relative flex h-full flex-col items-center justify-center gap-4 overflow-hidden rounded-xl border border-neutral-700/50 bg-gradient-to-br from-neutral-800/60 to-neutral-850/60 p-6 sm:p-8 text-center shadow-lg backdrop-blur-sm transition-all duration-300 hover:border-neutral-600/80",
                            `hover:shadow-2xl hover:${s.shadowColor}`
                          )}>
                            <div className={cn(
                              `absolute -top-1/4 -right-1/4 h-1/2 w-1/2 rounded-full opacity-10 blur-3xl bg-gradient-radial ${s.color}`
                            )}></div>
                            <div className={`relative z-10 rounded-full bg-gradient-to-br ${s.color} p-4 text-white shadow-lg ${s.shadowColor}`}>
                              {s.icon}
                            </div>
                            <CardHeader className="relative z-10 p-0 text-lg sm:text-xl font-semibold text-neutral-100">
                              {s.tier} Plan
                            </CardHeader>
                            <CardContent className="relative z-10 p-0">
                              <div className="text-4xl sm:text-5xl font-bold text-white">
                                {s.count}
                              </div>
                              <div className="mt-1 text-xs sm:text-sm text-neutral-400">
                                Subscribers
                              </div>
                            </CardContent>
                          </Card>
                        </motion.div>
                      ))}
                    </motion.div>
                  )}

                  {tab === 'users' && (
                    <motion.div
                      className="grid gap-4 sm:gap-6 md:grid-cols-2"
                      variants={containerVariants}
                      initial="hidden"
                      animate="visible"
                    >
                      <motion.div variants={itemVariants}>
                        <Card className="group overflow-hidden rounded-xl border border-neutral-700/50 bg-neutral-800/50 shadow-lg backdrop-blur-sm transition-all duration-300 hover:border-neutral-600/80 hover:shadow-xl hover:shadow-red-900/20">
                          <CardHeader className="border-b border-neutral-700/50 bg-gradient-to-b from-neutral-750/60 to-neutral-800/40 p-3 sm:px-6 sm:py-4">
                            <div className="flex items-center gap-2">
                              <Users className="h-5 w-5 text-red-500 transition-transform duration-300 group-hover:scale-110" />
                              <h3 className="text-base sm:text-lg font-semibold text-neutral-100">Top Similar User Pairs</h3>
                            </div>
                          </CardHeader>
                          <CardContent className="max-h-64 sm:max-h-72 overflow-y-auto p-0">
                            <ul className="divide-y divide-neutral-700/30">
                              {similarPairs.length > 0 ? similarPairs.map((p, i) => (
                                <li
                                  key={`${p.user1ID}-${p.user2ID}`}
                                  className="flex items-center justify-between px-4 py-3 sm:px-6 sm:py-3.5 transition-colors hover:bg-neutral-700/40"
                                >
                                  <div className="flex items-center gap-2 sm:gap-3">
                                    <div className="flex h-7 w-7 sm:h-8 sm:w-8 shrink-0 items-center justify-center rounded-full bg-neutral-700/60 text-xs font-medium text-neutral-300 ring-1 ring-neutral-600/50">
                                      {i + 1}
                                    </div>
                                    <span className="text-xs sm:text-sm text-neutral-200">
                                      User {p.user1ID} &amp; {p.user2ID}
                                    </span>
                                  </div>
                                  <div className="flex items-baseline gap-1 rounded-md bg-red-500/10 px-2 py-1 text-xs">
                                    <span className="font-medium text-red-400">
                                      {p.similarity.toFixed(1)}%
                                    </span>
                                    <span className="text-neutral-500">Similar</span>
                                  </div>
                                </li>
                              )) : (
                                <li className="p-6 text-center text-neutral-500">No similarity data available.</li>
                              )}
                            </ul>
                          </CardContent>
                        </Card>
                      </motion.div>

                      <motion.div variants={itemVariants}>
                        <Card className="group overflow-hidden rounded-xl border border-neutral-700/50 bg-neutral-800/50 shadow-lg backdrop-blur-sm transition-all duration-300 hover:border-neutral-600/80 hover:shadow-xl hover:shadow-red-900/20">
                          <CardHeader className="border-b border-neutral-700/50 bg-gradient-to-b from-neutral-750/60 to-neutral-800/40 p-3 sm:px-6 sm:py-4">
                            <div className="flex items-center gap-2">
                              <Activity className="h-5 w-5 text-red-500 transition-transform duration-300 group-hover:scale-110" />
                              <h3 className="text-base sm:text-lg font-semibold text-neutral-100">Most Active Users</h3>
                            </div>
                          </CardHeader>
                          <CardContent className="max-h-64 sm:max-h-72 overflow-y-auto p-0">
                            <ul className="divide-y divide-neutral-700/30">
                              {activeUsers.length > 0 ? activeUsers.map((u, i) => (
                                <li
                                  key={u.userID}
                                  className="flex items-center justify-between px-4 py-3 sm:px-6 sm:py-3.5 transition-colors hover:bg-neutral-700/40"
                                >
                                  <div className="flex items-center gap-2 sm:gap-3">
                                    <div className="flex h-8 w-8 sm:h-9 sm:w-9 shrink-0 items-center justify-center rounded-full bg-neutral-700/60 text-xs sm:text-sm font-medium uppercase text-neutral-300 ring-1 ring-neutral-600/50">
                                      {u.name.charAt(0)}
                                    </div>
                                    <div>
                                      <div className="font-medium text-sm sm:text-base text-neutral-100">{u.name}</div>
                                      <div className="text-xs text-neutral-400">
                                        Prefers: {u.genre}
                                      </div>
                                    </div>
                                  </div>
                                  <div className="flex items-baseline gap-1 rounded-md bg-red-500/10 px-2 py-1 text-xs">
                                    <span className="font-medium text-red-400">
                                      {u.watchTime.toFixed(1)}
                                    </span>
                                    <span className="text-neutral-500">hrs</span>
                                  </div>
                                </li>
                              )) : (
                                <li className="p-6 text-center text-neutral-500">No active user data available.</li>
                              )}
                            </ul>
                          </CardContent>
                        </Card>
                      </motion.div>
                    </motion.div>
                  )}

                  {tab === 'graph' && (
                     <GraphDisplay />
                  )}
                </motion.div>
              </AnimatePresence>
            </motion.div>
          </ScrollArea>
        </div>
      </main>
    </div>
  );
}
