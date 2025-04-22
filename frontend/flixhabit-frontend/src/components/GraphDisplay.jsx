import React, { useState, useEffect, useRef, useCallback, useLayoutEffect } from 'react';
import { motion } from 'framer-motion';
import ForceGraph2D from 'react-force-graph-2d'; 
import { Card, CardHeader, CardContent } from './card';
import { Loader2, AlertTriangle } from 'lucide-react';

export default function GraphDisplay() {
  const [graphData, setGraphData] = useState({ nodes: [], links: [] }); // Use 'links' for ForceGraph
  const [loading, setLoading] = useState(true);
  const [error, setError] = useState(null);
  const fgRef = useRef(); 
  const containerRef = useRef(); // Ref to get container dimensions
  const [dimensions, setDimensions] = useState({ width: 0, height: 0 });
  // Set default dimensions
  const DEFAULT_WIDTH = 800;
  const DEFAULT_HEIGHT = 500;
  const [isMobile, setIsMobile] = useState(false);

  // Check if device is mobile based on screen width
  useEffect(() => {
    const checkMobile = () => {
      const mobile = window.innerWidth < 768; 
      setIsMobile(mobile);
      console.log("[GraphDisplay] Is mobile device:", mobile);
    };
    
    // Check initially
    checkMobile();
    
    // Add resize listener
    window.addEventListener('resize', checkMobile);
    
    return () => window.removeEventListener('resize', checkMobile);
  }, []);

  // Use useLayoutEffect for initial measurement with retry 
  useLayoutEffect(() => {
    const measure = () => {
      if (containerRef.current) {
        const { width, height } = containerRef.current.getBoundingClientRect();
        console.log("[GraphDisplay] Attempting measurement, got:", { width, height });
        
        if (width > 0 && height > 0) {
          console.log("[GraphDisplay] Initial dimensions from useLayoutEffect:", { width, height });
          setDimensions({ width, height });
          return true; // Measurement successful
        } else {
          console.warn("[GraphDisplay] useLayoutEffect measured zero dimensions initially.");
        }
      } else {
        console.warn("[GraphDisplay] containerRef not set in useLayoutEffect");
      }
      return false;
    };

    const initialSuccess = measure();
    
    // If initial measurement fails, retry with setTimeout
    if (!initialSuccess) {
      console.log("[GraphDisplay] Initial measurement failed, will retry in 100ms");
      const timerId = setTimeout(() => {
        console.log("[GraphDisplay] Retrying measurement...");
        const retrySuccess = measure();
        
        if (!retrySuccess) {
          console.warn("[GraphDisplay] Retry failed, using default dimensions", 
                       { width: DEFAULT_WIDTH, height: DEFAULT_HEIGHT });
          setDimensions({ width: DEFAULT_WIDTH, height: DEFAULT_HEIGHT });
        }
      }, 100);
      
      return () => clearTimeout(timerId); 
    }
  }, []); 

  // Keep ResizeObserver effect for dimension updates
  useEffect(() => {
    const observer = new ResizeObserver(entries => {
      if (entries[0]) {
        const { width, height } = entries[0].contentRect;
        if (width > 0 && height > 0) { 
          console.log("[GraphDisplay] ResizeObserver updated dimensions:", { width, height });
          setDimensions(prev => {
            if (prev.width !== width || prev.height !== height) {
              return { width, height };
            }
            return prev;
          });
        } else {
          console.log("[GraphDisplay] ResizeObserver reported zero dimensions, ignoring.");
        }
      }
    });

    const currentElement = containerRef.current;
    if (currentElement) {
      console.log("[GraphDisplay] Observing container ref:", currentElement);
      observer.observe(currentElement);
    }

    return () => {
      if (currentElement && currentElement.isConnected) { // Check if element is still in DOM
        console.log("[GraphDisplay] Disconnecting observer");
        observer.unobserve(currentElement);
      }
    };
  }, []);

  useEffect(() => {
    const fetchGraphData = async () => {
      setLoading(true);
      setError(null);
      setGraphData({ nodes: [], links: [] }); // Reset data on fetch
      try {
        const res = await fetch('/data/genre_graph.json'); 
        if (!res.ok) {
          throw new Error(`Could not fetch genre_graph.json (status: ${res.status})`);
        }
        const data = await res.json();

        if (!data || !Array.isArray(data.nodes) || !Array.isArray(data.edges)) {
           throw new Error('Invalid data structure in genre_graph.json. Expected object with "nodes" and "edges" arrays.');
        }

        const links = data.edges.map(edge => ({ source: edge.from, target: edge.to }));
        const finalGraphData = { nodes: data.nodes, links };
        console.log("[GraphDisplay] Fetched and processed graph data:", finalGraphData); // Log processed data
        setGraphData(finalGraphData);

      } catch (e) {
        console.error("Error fetching graph data:", e);
        setError(e.message);
      } finally {
        setLoading(false);
      }
    };

    fetchGraphData();
  }, []);


  const drawNode = useCallback((node, ctx) => {
    // Adjust node size based on device
    const NODE_SIZE = isMobile ? 10 : 8; 
    
    // Draw circle with border
    ctx.beginPath();
    ctx.arc(node.x, node.y, NODE_SIZE, 0, 2 * Math.PI, false);
    ctx.fillStyle = '#e50914';
    ctx.fill();
    ctx.strokeStyle = '#ffffff';
    ctx.lineWidth = 1.5; 
    ctx.stroke();
    
    // Improve label visibility - adjust for mobile
    const fontSize = isMobile ? 11 : 10;
    const labelOffset = isMobile ? 18 : 16;
    
    ctx.font = `bold ${fontSize}px Inter, sans-serif`;
    ctx.textAlign = 'center';
    ctx.textBaseline = 'middle';
    ctx.fillStyle = '#ffffff';
    
    // Enhanced text shadow for better readability, especially on mobile
    ctx.shadowColor = 'rgba(0, 0, 0, 0.8)';
    ctx.shadowBlur = 5;
    ctx.shadowOffsetX = 0;
    ctx.shadowOffsetY = 0;
    ctx.fillText(node.label, node.x, node.y + labelOffset);
    
    // Reset shadow
    ctx.shadowBlur = 0;
    ctx.shadowOffsetX = 0;
    ctx.shadowOffsetY = 0;
  }, [isMobile]);
  
  // Loading State
  if (loading) {
    return (
      <div className="flex h-96 items-center justify-center rounded-xl border border-neutral-700/50 bg-neutral-800/50 p-6 text-neutral-400">
        <Loader2 className="mr-2 h-5 w-5 animate-spin" />
        Loading Genre Graph...
      </div>
    );
  }

  // Error State
  if (error) {
    return (
      <div className="flex h-96 flex-col items-center justify-center rounded-xl border border-red-700/50 bg-red-900/20 p-6 text-red-400">
        <AlertTriangle className="mb-2 h-8 w-8" />
        <p className="font-semibold">Error loading graph</p>
        <p className="text-sm text-neutral-400 text-center">{error}</p>
      </div>
    );
  }
  
  // Empty Data State
  if (graphData.nodes.length === 0) {
     return (
      <div className="flex h-96 items-center justify-center rounded-xl border border-neutral-700/50 bg-neutral-800/50 p-6 text-neutral-500">
        No nodes found in genre_graph.json.
      </div>
    );
  }

  // Graph Display
  return (
    <motion.div
      initial={{ opacity: 0 }}
      animate={{ opacity: 1 }}
      transition={{ duration: 0.5 }}
    >
      <Card className="group relative overflow-hidden rounded-xl border border-neutral-700/50 bg-neutral-800/50 shadow-lg backdrop-blur-sm">
        <CardHeader className="border-b border-neutral-700/50 bg-gradient-to-b from-neutral-750/60 to-neutral-800/40 px-6 py-4">
          <h3 className="text-lg font-semibold text-neutral-100">Genre Connections Graph</h3>
        </CardHeader>
        {/* Container for ForceGraph size reference */}
        <CardContent 
          ref={containerRef} 
          className={`relative p-0 ${isMobile ? 'h-[400px]' : 'h-[500px]'}`}
        > 
           {/* Log dimensions state before rendering */}
           {console.log("[GraphDisplay] Rendering with dimensions:", dimensions, "isMobile:", isMobile)} 
          {dimensions.width > 0 && dimensions.height > 0 ? (
            <ForceGraph2D
              ref={fgRef}
              graphData={graphData}
              width={dimensions.width}
              height={dimensions.height}
              nodeId="id"
              nodeLabel="label"
              nodeCanvasObject={drawNode}
              nodeCanvasObjectMode={() => 'after'}
              linkSource="source"
              linkTarget="target"
              linkColor={() => 'rgba(255, 255, 255, 0.4)'} // Increased opacity for better visibility
              linkWidth={isMobile ? 2 : 1.5} // Thicker on mobile
              linkDirectionalParticles={isMobile ? 0 : 2} 
              linkDirectionalParticleWidth={2}
              linkDirectionalParticleColor={() => '#e50914'}
              linkDirectionalParticleSpeed={0.006}
              enableZoomInteraction={false}
              enablePanInteraction={true}
              enablePointerInteraction={true}
              backgroundColor="rgba(0,0,0,0)"
              cooldownTicks={100}
              minZoom={2}
              maxZoom={3}
              zoom={2}
              // Force configuration - adjusted for better spacing 
              d3AlphaDecay={0.02} 
              d3VelocityDecay={0.2} 
              // Increase distance between nodes
              d3Force={isMobile ? 
                // Mobile-specific force configuration
                (d3, nodes, links) => {
                  // Create stronger repulsion between nodes
                  d3.forceSimulation(nodes)
                    .force('link', d3.forceLink(links).id(d => d.id).distance(70))
                    .force('charge', d3.forceManyBody().strength(-200))
                    .force('center', d3.forceCenter(dimensions.width / 2, dimensions.height / 2))
                    .force('collision', d3.forceCollide(25)); // Prevent node overlap
                } : 
                // Desktop force configuration
                (d3, nodes, links) => {
                  d3.forceSimulation(nodes)
                    .force('link', d3.forceLink(links).id(d => d.id).distance(50))
                    .force('charge', d3.forceManyBody().strength(-120))
                    .force('center', d3.forceCenter(dimensions.width / 2, dimensions.height / 2))
                    .force('collision', d3.forceCollide(15));
                }
              }
            />
          ) : (
             <div className="flex items-center justify-center h-full text-neutral-500 text-sm">
               {loading ? 'Loading graph data...' : 'Waiting for container dimensions...'}
             </div>
          )}
        </CardContent>
      </Card>
    </motion.div>
  );
} 